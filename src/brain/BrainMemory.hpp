#pragma once

#include "../core/Types/Types.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/AssociativeMemory.hpp"
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

class BrainMemory {
public:
    // ===========================================
    // WORKING MEMORY SYSTEM
    // ===========================================
    
    static void initializeWorkingMemory(Brain* brain, NeuralWorkingMemory* workingMemory,
                                         size_t capacity);
    static void updateWorkingMemory(NeuralWorkingMemory* workingMemory, TimestepDuration dt);
    static void storeActiveNeuronsToWorkingMemory(NeuralWorkingMemory* workingMemory,
                                                  const std::vector<Neuron*>& neurons);
    
    // ===========================================
    // EPISODIC MEMORY SYSTEM
    // ===========================================
    
    static void updateEpisodicMemory(NeuralEpisodicMemory* episodicMemory,
                                     size_t currentStep, float reward);
    static void replayMemories(NeuralEpisodicMemory* episodicMemory, size_t replayCount);
    static void consolidateMemories(NeuralEpisodicMemory* episodicMemory, float consolidationRate);
    
    // ===========================================
    // ASSOCIATIVE MEMORY SYSTEM
    // ===========================================
    
    static void initializeAssociativeMemory(NeuralAssociativeMemory* associativeMemory,
                                            Brain* brain);
    static void storeAssociations(NeuralAssociativeMemory* associativeMemory,
                                  const std::vector<Neuron*>& neurons, float strength);
    
    // ===========================================
    // MEMORY STATISTICS AND ACCESSORS
    // ===========================================
    
    static void logMemoryStatus(NeuralWorkingMemory* workingMemory,
                                NeuralEpisodicMemory* episodicMemory,
                                NeuralAssociativeMemory* associativeMemory);
};

} // namespace nlm