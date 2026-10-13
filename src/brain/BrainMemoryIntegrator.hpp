#pragma once

#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "BrainInitializer.hpp"
#include <memory>
#include <vector>

namespace nlm {

class Brain;
class Neuron;

class BrainMemoryIntegrator {
public:
    explicit BrainMemoryIntegrator(std::shared_ptr<Config> config);
    ~BrainMemoryIntegrator() = default;
    
    // Initialize memory systems
    void initializeWorkingMemory(Brain* brain, size_t neuronCount);
    void initializeEpisodicMemory(Brain* brain);
    void initializeAssociativeMemory(Brain* brain);
    
    // Initialize prediction system
    void initializePredictionSystem(Brain* brain);
    
    // Update memory systems from neural activity
    void updateMemoryFromActivity(Brain* brain, SimulationStep step, Timestamp time);
    void storeToWorkingMemory(Brain* brain, NeuronId neuronId, float activation);
    
    // Episodic memory management
    void storeActiveNeuronsToEpisodicMemory(Brain* brain, SimulationStep step, Timestamp time);
    void replayMemories(Brain* brain, SimulationStep step, size_t maxEpisodes = 3);
    void consolidateMemories(Brain* brain, float consolidationFactor);
    
    // Memory statistics and access
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    
    // Memory system configuration
    void configureWorkingMemoryCapacity(Brain* brain, size_t capacity);
    void configureEpisodicMemoryMaxEpisodes(Brain* brain, size_t maxEpisodes);
    void configureAssociativeMemoryCapacity(Brain* brain, size_t capacity);
    
    // State management
    void resetMemorySystems(Brain* brain);
    void clearWorkingMemory(Brain* brain);
    void clearEpisodicMemory(Brain* brain);
    void clearAssociativeMemory(Brain* brain);

private:
    // Memory system instances
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Configuration parameters
    size_t workingMemoryCapacity;
    size_t episodicMemoryMaxEpisodes;
    size_t associativeMemoryCapacity;
    
    // Helper methods
    void setupMemoryConnections(Brain* brain);
    void updateWorkingMemoryFromNeuralActivity(Brain* brain);
    void captureCurrentStateForEpisodicMemory(Brain* brain, SimulationStep step, Timestamp time);
    void processReplayBatch(Brain* brain, size_t batchSize);
    void consolidateWeakMemories(Brain* brain, float consolidationFactor);
    
    // Neuromodulation integration
    void integrateNeuromodulation(Brain* brain);
    float getModulatedLearningRate(Brain* brain, float baseRate);
};