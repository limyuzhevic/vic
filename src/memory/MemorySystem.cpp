#include "MemorySystem.hpp"
#include <algorithm>
#include <string>

namespace nlm {

MemorySystem::MemorySystem()
    : workingMemory(std::make_unique<WorkingMemory>())
    , episodicMemory(std::make_unique<EpisodicMemory>())
    , semanticMemory(std::make_unique<SemanticMemory>())
    , proceduralMemory(std::make_unique<ProceduralMemory>())
    , associativeMemory(std::make_unique<AssociativeMemory>()) {}

MemorySystem::~MemorySystem() = default;

WorkingMemory& MemorySystem::getWorkingMemory() {
    return *workingMemory;
}

const WorkingMemory& MemorySystem::getWorkingMemory() const {
    return *workingMemory;
}

EpisodicMemory& MemorySystem::getEpisodicMemory() {
    return *episodicMemory;
}

const EpisodicMemory& MemorySystem::getEpisodicMemory() const {
    return *episodicMemory;
}

SemanticMemory& MemorySystem::getSemanticMemory() {
    return *semanticMemory;
}

const SemanticMemory& MemorySystem::getSemanticMemory() const {
    return *semanticMemory;
}

ProceduralMemory& MemorySystem::getProceduralMemory() {
    return *proceduralMemory;
}

const ProceduralMemory& MemorySystem::getProceduralMemory() const {
    return *proceduralMemory;
}

AssociativeMemory& MemorySystem::getAssociativeMemory() {
    return *associativeMemory;
}

const AssociativeMemory& MemorySystem::getAssociativeMemory() const {
    return *associativeMemory;
}

std::string MemorySystem::getMemoryStats() const {
    std::ostringstream oss;
    oss << "Memory System Statistics:\n";
    oss << "Working Memory: " << workingMemory->getCurrentSize() << "/" 
        << workingMemory->getCapacity() << " items\n";
    oss << "Episodic Memory: " << episodicMemory->getEpisodeCount() << " episodes\n";
    oss << "Semantic Memory: " << semanticMemory->getAllFacts().size() << " facts\n";
    oss << "Procedural Memory: " << proceduralMemory->getAllSkills().size() << " skills\n";
    oss << "Associative Memory: " << associativeMemory->getAssociationCount() << " associations\n";
    return oss.str();
}

void MemorySystem::clear() {
    workingMemory->clear();
    episodicMemory->clear();
    semanticMemory->clear();
    proceduralMemory->clear();
    associativeMemory->clear();
}

} // namespace nlm
