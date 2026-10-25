#ifndef MEMORY_SYSTEM_H
#define MEMORY_SYSTEM_H

#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void updateWorkingMemory(TimestepDuration timestep) = 0;
    virtual void storeEpisodicMemory(const EpisodicMemoryItem& episode) = 0;
    virtual void updateEpisodicMemory(const BrainContext& context, SimulationStep currentStep,
                                    const NeuromodulatorSignal& rewardSignal) = 0;
    virtual void consolidateMemories(float consolidationThreshold) = 0;
    
    virtual NeuralWorkingMemory* getWorkingMemory() = 0;
    virtual NeuralEpisodicMemory* getEpisodicMemory() = 0;
    virtual NeuralAssociativeMemory* getAssociativeMemory() = 0;
    
    virtual void replayMemories(size_t maxEpisodes) = 0;
    virtual size_t getEpisodeCount() const = 0;
    
protected:
    Brain* brain_ = nullptr;
};

} // namespace nlm

#endif // MEMORY_SYSTEM_H