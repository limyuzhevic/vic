#pragma once

#include "../brain/Brain.hpp"
#include "WorkingMemory.hpp"
#include "EpisodicMemory.hpp"
#include "SemanticMemory.hpp"
#include "ProceduralMemory.hpp"
#include "AssociativeMemory.hpp"
#include <string>

namespace nlm {

class MemorySystem {
public:
    MemorySystem();
    ~MemorySystem();
    
    // Get memory system components
    WorkingMemory& getWorkingMemory();
    const WorkingMemory& getWorkingMemory() const;
    
    EpisodicMemory& getEpisodicMemory();
    const EpisodicMemory& getEpisodicMemory() const;
    
    SemanticMemory& getSemanticMemory();
    const SemanticMemory& getSemanticMemory() const;
    
    ProceduralMemory& getProceduralMemory();
    const ProceduralMemory& getProceduralMemory() const;
    
    AssociativeMemory& getAssociativeMemory();
    const AssociativeMemory& getAssociativeMemory() const;
    
    // Get memory system statistics
    std::string getMemoryStats() const;
    
    // Clear all memory
    void clear();
    
private:
    std::unique_ptr<WorkingMemory> workingMemory;
    std::unique_ptr<EpisodicMemory> episodicMemory;
    std::unique_ptr<SemanticMemory> semanticMemory;
    std::unique_ptr<ProceduralMemory> proceduralMemory;
    std::unique_ptr<AssociativeMemory> associativeMemory;
};

} // namespace nlm
