#pragma once

#include "../brain/Brain.hpp"

namespace nlm {

class BrainMemoryManagement {
public:
    BrainMemoryManagement(Brain::Impl* pImpl) : pImpl(pImpl) {}
    
    // Update working memory state
    void updateWorkingMemory();
    
    // Update episodic memory with current experience
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    
    // Periodic memory consolidation
    void consolidateMemories(SimulationStep currentStep);
    
    // Memory replay for consolidation
    void replayMemories(SimulationStep currentStep);

private:
    Brain::Impl* pImpl;
};

} // namespace nlm