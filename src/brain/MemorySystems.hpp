// MemorySystems.hpp
// Memory systems interface

#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

class MemorySystems {
public:
    void initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain);
    void processStep(SimulationStep currentStep, Timestamp currentTime);
    void storeEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    void replayMemories();
    void consolidateMemories(float threshold);
    
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
private:
    void storeEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    float getCurrentReward() const;
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    size_t workingMemoryCapacity_;
    size_t episodicMemoryMaxEpisodes_;
    size_t episodeStorageInterval_ = 10;
    size_t memoryReplayCount_ = 3;
};

} // namespace nlm
