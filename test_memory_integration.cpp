#include "Brain.hpp"
#include "Memory.hpp"

// Testing memory systems integration

int main() {
    // Test that Brain::getWorkingMemory() returns functional object
    auto brain = std::make_shared<nnlm::Brain>(std::make_shared<nnlm::Config>());
    
    if (!brain->initialize()) {
        std::cout << "Brain initialization failed" << std::endl;
        return 1;
    }
    
    // Test working memory
    auto* workingMem = brain->getWorkingMemory();
    if (!workingMem) {
        std::cout << "Working memory is null" << std::endl;
        return 1;
    }
    std::cout << "Working memory initialized: " << workingMem->getActiveTraces() << " traces" << std::endl;
    
    // Test episodic memory
    auto* episodicMem = brain->getEpisodicMemory();
    if (!episodicMem) {
        std::cout << "Episodic memory is null" << std::endl;
        return 1;
    }
    std::cout << "Episodic memory initialized: " << episodicMem->getEpisodeCount() << " episodes" << std::endl;
    
    // Test associative memory
    auto* associativeMem = brain->getAssociativeMemory();
    if (!associativeMem) {
        std::cout << "Associative memory is null" << std::endl;
        return 1;
    }
    std::cout << "Associative memory initialized: " << associativeMem->getAssociationCount() << " associations" << std::endl;
    
    std::cout << "All memory systems are functional!" << std::endl;
    return 0;
}