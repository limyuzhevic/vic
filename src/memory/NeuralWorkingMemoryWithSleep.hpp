#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/SleepState.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace nlm {

// Sleep-enhanced working memory with state-specific decay and replay
class NeuralWorkingMemoryWithSleep {
public:
    NeuralWorkingMemoryWithSleep();
    ~NeuralWorkingMemoryWithSleep();
    
    // Initialize with sleep state manager reference
    void initialize(Brain* brain);
    
    // Store information in working memory with sleep state awareness
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;
    
    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;
    
    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;
    
    // Update working memory (maintenance and decay) with sleep state
    void update(TimestepDuration dt, SleepState sleepState);
    
    // Clear working memory
    void clear();
    
    // Get number of active memory traces
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    // Sleep state-specific decay rates
    float getDecayRate(SleepState state) const;
    void setDecayRate(SleepState state, float rate);
    
    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);
    
    // Competition between memory traces
    void runCompetition();
    
    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;
    
    // Get overall memory activity level
    float getMemoryActivity() const;
    
    // Sleep replay and consolidation methods
    void executeSleepReplay();
    void consolidateDuringSleep(SleepState sleepState, float duration);
    void patternCompletionDuringSleep(SleepState sleepState);
    
    // Check if working memory is active
    bool isActive() const { return !memoryNeurons_.empty(); }
    
    // Get memory statistics
    float getMemoryStabilization() const { return memoryStabilization_; }
    float getReplayStrength() const { return replayStrength_; }
    
    // State management
    SleepState getCurrentSleepState() const { return currentSleepState_; }
    void setCurrentSleepState(SleepState state) { currentSleepState_ = state; }
    
    // Checkpointing support
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    
    // Update recurrent connections for maintenance
    void updateRecurrentConnections();
    
    // Decay weak memory traces
    void decayWeakTraces();
    
    // Apply state-specific decay
    void applyStateSpecificDecay(SleepState state, float dt);
    
    // Sleep state-specific replay patterns
    void executeQuietSleepReplay();
    void executeDeepSleepReplay();
    void executeREMSleepReplay();
    
    // Pattern completion during different sleep states
    void completePatternsQuietSleep();
    void completePatternsDeepSleep();
    void completePatternsREMSleep();
    
    // Integration with state management for active/passive states
    void integrateWithStateManagement(SleepState sleepState, float dt);
    void handleStateTransition(SleepState oldState, SleepState newState);
    
    // Helper functions for sleep-specific processing
    float calculateReplayProbability(SleepState state, float memoryAge) const;
    float getStateSpecificReconstructionStrength(SleepState state) const;
    bool shouldReplayDuringState(SleepState state, float memoryImportance) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
    
    // Sleep state management
    SleepState currentSleepState_;
    std::map<SleepState, float> sleepStateDecayRates_;
    std::map<SleepState, float> sleepStateReplayStrengths_;
    std::map<SleepState, float> sleepStateCompletionFactors_;
    
    // Sleep replay statistics
    float memoryStabilization_;
    float replayStrength_;
    float consolidationProgress_;
    
    // Replay scheduling
    float nextReplayTime_;
    float replayInterval_;
    size_t replayCount_;
};

} // namespace nlm
