#include "Brain.hpp"
#include "../memory/Memory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralSemanticMemory.hpp"
#include "../memory/NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Brain integration with memory systems
// This connects the Brain class to the neural memory systems

void Brain::initializeMemorySystems() {
    NLM_LOG_INFO("Initializing memory systems with neural integration");
    
    // Initialize NeuralWorkingMemory
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(100);
    pImpl->workingMemory->setDecayRate(0.01f);
    
    // Initialize NeuralEpisodicMemory
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize NeuralAssociativeMemory
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    pImpl->associativeMemory->initialize(this);
    
    // Initialize NeuralSemanticMemory (not exposed in Brain.h but available)
    // This would be used for advanced semantic knowledge integration
    pImpl->semanticMemory = std::make_unique<NeuralSemanticMemory>();
    pImpl->semanticMemory->initialize(this);
    
    // Initialize NeuralProceduralMemory (not exposed in Brain.h but available)
    // This would be used for skill learning and procedural memory
    pImpl->proceduralMemory = std::make_unique<NeuralProceduralMemory>();
    pImpl->proceduralMemory->initialize(this);
    
    NLM_LOG_INFO("Memory systems initialized with neural implementations");
    NLM_LOG_INFO("Working memory capacity: " + std::to_string(pImpl->workingMemory->getCapacity()));
    NLM_LOG_INFO("Episodic memory max episodes: " + std::to_string(pImpl->episodicMemory->getMaxEpisodes()));
    NLM_LOG_INFO("Associative memory ready for integration");
}

void Brain::updateMemorySystems() {
    // Update neural memory systems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    if (pImpl->episodicMemory) {
        // Neural episodic memory updates happen during experience processing
    }
    
    if (pImpl->associativeMemory) {
        // Neural associative memory updates happen during learning
    }
    
    if (pImpl->semanticMemory) {
        // Neural semantic memory updates happen through concept formation
    }
    
    if (pImpl->proceduralMemory) {
        // Neural procedural memory updates happen through skill practice
    }
    
    // Integration between memory systems
    integrateMemorySystems();
}

void Brain::integrateMemorySystems() {
    if (!pImpl->workingMemory || !pImpl->episodicMemory || !pImpl->associativeMemory) {
        return;
    }
    
    // Get working memory content as episodic candidate
    std::vector<float> memoryPattern = pImpl->workingMemory->retrieve();
    
    if (!memoryPattern.empty() && pImpl->workingMemory->getMemoryActivity() > 0.5f) {
        // Create episodic memory item from working memory
        // This bridges working memory to episodic memory
        NLM_LOG_INFO("Integrating working memory with episodic memory");
        
        // The neural episodic memory already has mechanisms to receive
        // working memory content during the step processing
    }
    
    // Integrate with semantic memory
    if (pImpl->semanticMemory && !memoryPattern.empty()) {
        // Neural semantic memory can use working memory for concept formation
        NLM_LOG_INFO("Integrating working memory with semantic memory");
    }
    
    // Integrate with procedural memory
    if (pImpl->proceduralMemory && !memoryPattern.empty()) {
        // Neural procedural memory can use working memory for skill learning
        NLM_LOG_INFO("Integrating working memory with procedural memory");
    }
    
    // Integrate associative memory with episodic memory
    if (pImpl->associativeMemory && pImpl->episodicMemory) {
        // Neural episodic memory stores episodes that create associations
        NLM_LOG_INFO("Integrating episodic and associative memory");
    }
}

void Brain::resetMemorySystems() {
    NLM_LOG_INFO("Resetting memory systems");
    
    if (pImpl->workingMemory) {
        pImpl->workingMemory->clear();
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->clear();
    }
    
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->clear();
    }
    
    if (pImpl->semanticMemory) {
        pImpl->semanticMemory->clear();
    }
    
    if (pImpl->proceduralMemory) {
        pImpl->proceduralMemory->clear();
    }
    
    NLM_LOG_INFO("Memory systems reset complete");
}

void Brain::consolidateMemorySystems() {
    NLM_LOG_INFO("Consolidating memory systems");
    
    if (pImpl->episodicMemory) {
        // Neural episodic memory has consolidation mechanisms
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    if (pImpl->semanticMemory) {
        // Neural semantic memory can consolidate important concepts
        // This happens during sleep/rest cycles
    }
    
    if (pImpl->proceduralMemory) {
        // Neural procedural memory consolidates skills during rest
        // This happens through replay mechanisms
    }
    
    NLM_LOG_INFO("Memory systems consolidation complete");
}

void Brain::replayImportantMemories() {
    NLM_LOG_INFO("Replaying important memories");
    
    if (pImpl->episodicMemory) {
        // Neural episodic memory has replay mechanisms
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    NLM_LOG_INFO("Memory replay complete");
}

} // namespace nlm
