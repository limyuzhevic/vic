#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class MemoryIntegrationManager {
public:
    MemoryIntegrationManager(std::shared_ptr<Config> config);
    ~MemoryIntegrationManager();
    
    // Initialize memory systems
    bool initialize(Brain* brain);
    
    // Main memory integration phase
    void update(Timestamp time, TimestepDuration timestep);
    
    // Replay important memories
    void replay(Timestamp time, TimestepDuration timestep);
    
    // Reset memory systems
    void reset();
    
    // Get component access
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
    // Memory statistics
    size_t getActiveMemoryTraces() const;
    size_t getEpisodeCount() const;
    size_t getMemoryNeurons() const;
    
    // Memory consolidation and cleanup
    void consolidate(float consolidationStrength);
    void clearWeakMemories(float threshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm