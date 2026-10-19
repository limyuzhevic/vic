#include "MemorySystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class MemorySystem::Impl {
public:
    Brain* brain;
    
    std::unique_ptr<WorkingMemory> workingMemory;
    std::unique_ptr<EpisodicMemory> episodicMemory;
    std::unique_ptr<SemanticMemory> semanticMemory;
    std::unique_ptr<ProceduralMemory> proceduralMemory;
    std::unique_ptr<AssociativeMemory> associativeMemory;
    
    Impl(Brain* b) : brain(b) {}
};

MemorySystem::MemorySystem(Brain* brain)
    : pImpl(new Impl(brain))
{
    NLM_LOG_INFO("MemorySystem created");
}

MemorySystem::~MemorySystem() = default;

void MemorySystem::initialize() {
    // Initialize memory levels
    pImpl->workingMemory = std::make_unique<WorkingMemory>();
    pImpl->episodicMemory = std::make_unique<EpisodicMemory>();
    pImpl->semanticMemory = std::make_unique<SemanticMemory>();
    pImpl->proceduralMemory = std::make_unique<ProceduralMemory>();
    pImpl->associativeMemory = std::make_unique<AssociativeMemory>();
    
    // Link memories together
    pImpl->workingMemory->updateFromBrain(pImpl->brain);
    pImpl->episodicMemory->updateFromBrain(pImpl->brain);
    pImpl->semanticMemory->updateFromBrain(pImpl->brain);
    pImpl->proceduralMemory->updateFromBrain(pImpl->brain);
    pImpl->associativeMemory->updateFromBrain(pImpl->brain);
}

void MemorySystem::updateAll(SimulationStep step, TimestepDuration dt) {
    // Update all memory systems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(dt);
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->advanceTime(1);
    }
    
    // Apply higher-level memories to brain
    if (pImpl->semanticMemory) {
        pImpl->semanticMemory->applyToBrain(pImpl->brain);
    }
    
    // Apply lower-level memories from brain
    if (pImpl->workingMemory && pImpl->brain) {
        pImpl->workingMemory->updateFromBrain(pImpl->brain);
    }
}

void MemorySystem::consolidateAll(float relevanceThreshold) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->consolidate(relevanceThreshold);
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(relevanceThreshold);
    }
    
    if (pImpl->semanticMemory) {
        pImpl->semanticMemory->consolidate(relevanceThreshold);
    }
}

void MemorySystem::replayAll() const {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->applyToBrain(pImpl->brain);
    }
}

void MemorySystem::clearAll() {
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->semanticMemory) pImpl->semanticMemory->clear();
    if (pImpl->proceduralMemory) pImpl->proceduralMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
}

void MemorySystem::propagateToLowerLevels(const std::vector<float>& pattern, float strength) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->store(pattern, strength);
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->store(pattern, strength);
    }
}

void MemorySystem::propagateToHigherLevels(const std::vector<float>& pattern, float strength) {
    if (pImpl->semanticMemory) {
        pImpl->semanticMemory->store(pattern, strength);
    }
    
    if (pImpl->proceduralMemory) {
        pImpl->proceduralMemory->store(pattern, strength);
    }
}

float MemorySystem::getSystemLoad() const {
    float total = 0.0f;
    
    if (pImpl->workingMemory) {
        total += pImpl->workingMemory->getLoad();
    }
    
    if (pImpl->episodicMemory) {
        total += pImpl->episodicMemory->getLoad();
    }
    
    if (pImpl->semanticMemory) {
        total += pImpl->semanticMemory->getLoad();
    }
    
    return total / 3.0f;  // Average of available memories
}

size_t MemorySystem::getTotalMemoryUsage() const {
    size_t total = 0;
    
    if (pImpl->workingMemory) {
        total += pImpl->workingMemory->getCurrentSize();
    }
    
    if (pImpl->episodicMemory) {
        total += pImpl->episodicMemory->getCurrentSize();
    }
    
    if (pImpl->semanticMemory) {
        total += pImpl->semanticMemory->getCurrentSize();
    }
    
    return total;
}

} // namespace nlm