#pragma once

#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/NeuralWorkingMemory.hpp"
#include "brain/NeuralEpisodicMemory.hpp"
#include "brain/NeuralAssociativeMemory.hpp"
#include "brain/Dopamine.hpp"
#include <vector>

namespace nlm {

class BrainMemory {
public:
    static void updateWorkingMemory(NeuralWorkingMemory* workingMemory,
                                   std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                   TimestepDuration timestep);
    
    static void updateEpisodicMemory(NeuralEpisodicMemory* episodicMemory,
                                   std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                   SimulationStep currentStep,
                                   size_t stepsSinceLastEpisode,
                                   Dopamine* dopamine);
    
    static void handleReplayAndConsolidation(NeuralEpisodicMemory* episodicMemory,
                                           size_t currentStep,
                                           size_t replayInterval,
                                           size_t consolidationInterval);
    
    static void initializeMemorySystems(NeuralWorkingMemory* workingMemory,
                                        NeuralEpisodicMemory* episodicMemory,
                                        NeuralAssociativeMemory* associativeMemory,
                                        size_t neuronCount,
                                        Brain* brain);
};

} // namespace nlm
