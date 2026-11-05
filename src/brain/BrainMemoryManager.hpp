// BrainMemoryManager.hpp - Memory systems management
#pragma once

#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;
class Neuron;

class BrainMemoryManager {
public:
    static void initializeWorkingMemory(Brain& brain);
    static void initializeEpisodicMemory(Brain& brain);
    static void initializeAssociativeMemory(Brain& brain);
    static void updateWorkingMemory(Brain& brain);
    static void storeEpisodicMemory(Brain& brain, SimulationStep currentStep);
    static void replayMemories(Brain& brain, SimulationStep currentStep);
    static void consolidateMemories(Brain& brain, SimulationStep currentStep);
    static void clearAllMemories(Brain& brain);
    static void restoreWorkingMemorySnapshot(Brain& brain);
    static void restoreEpisodicMemoryEpisodes(Brain& brain);

private:
    static void captureActiveNeurons(Brain& brain, const EpisodicMemoryItem& episode);
    static float calculateNeuronActivation(Brain& brain, const Neuron* neuron);
};

