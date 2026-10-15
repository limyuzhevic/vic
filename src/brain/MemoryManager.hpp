#pragma once

#include "../core/Types/Types.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"

namespace nlm {

class MemoryManager {
public:
    MemoryManager(std::shared_ptr<NeuralWorkingMemory> workingMemory,
                 std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                 std::shared_ptr<NeuralAssociativeMemory> associativeMemory);
    
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    void update(SimulationStep currentStep, Timestamp currentTime);
    float getEpisodicMemoryLoad() const;
    bool hasConsolidationWork() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
