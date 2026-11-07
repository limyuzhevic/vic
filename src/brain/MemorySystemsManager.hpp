#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class MemorySystemsManager {
public:
    MemorySystemsManager(Brain* brain);
    ~MemorySystemsManager();
    
    bool initialize();
    void step(double timestep);
    void reset();
    
    // Working memory access
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    void storeToWorkingMemory(NeuronId neuron, float activation);
    std::vector<NeuronId> getWorkingMemoryNeurons() const;
    
    // Episodic memory access
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    void storeEpisode(const EpisodicMemoryItem& episode);
    std::vector<EpisodicMemoryItem*> getEpisodesForReplay(size_t count);
    
    // Associative memory access
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
    // Memory system updates
    void updateWorkingMemory(double timestep);
    void updateEpisodicMemory(SimulationStep currentStep);
    void consolidateMemories(float threshold);
    
private:
    Brain* brain_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
};

} // namespace nlm
