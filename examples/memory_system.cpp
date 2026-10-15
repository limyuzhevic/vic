// Memory system example
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "Memory system example" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Access memory systems
    auto* extensibleBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    if (extensibleBrain) {
        // Access working memory
        auto* workingMemory = extensibleBrain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "Working memory capacity: " << workingMemory->getCapacity() << std::endl;
            std::cout << "Active traces: " << workingMemory->getActiveTraces() << std::endl;
            
            // Store some data in working memory
            for (int i = 0; i < 50; ++i) {
                workingMemory->store(static_cast<float>(i), static_cast<float>(i));
            }
            
            std::cout << "Stored 50 traces in working memory" << std::endl;
            
            // Retrieve some data from working memory
            float retrieved = workingMemory->retrieve(10.0f);
            std::cout << "Retrieved from working memory: " << retrieved << std::endl;
        }
        
        // Access episodic memory
        auto* episodicMemory = extensibleBrain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "Episodic memory max episodes: " << episodicMemory->getMaxEpisodes() << std::endl;
            std::cout << "Current episodes: " << episodicMemory->getEpisodeCount() << std::endl;
            
            // Store some data in episodic memory
            for (int i = 0; i < 100; ++i) {
                episodicMemory->store(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
            }
            
            std::cout << "Stored 100 episodes in episodic memory" << std::endl;
            
            // Retrieve some data from episodic memory
            float retrieved = episodicMemory->retrieve(10.0f, 5.0f);
            std::cout << "Retrieved from episodic memory: " << retrieved << std::endl;
        }
        
        // Access associative memory
        auto* associativeMemory = extensibleBrain->getAssociativeMemory();
        if (associativeMemory) {
            std::cout << "Associative memory max associations: " << associativeMemory->getMaxAssociations() << std::endl;
            std::cout << "Current associations: " << associativeMemory->getAssociationCount() << std::endl;
            
            // Store some associations in associative memory
            for (int i = 0; i < 200; ++i) {
                associativeMemory->store(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i) * 0.1f);
            }
            
            std::cout << "Stored 200 associations in associative memory" << std::endl;
            
            // Retrieve some associations from associative memory
            std::vector<float> retrieved = associativeMemory->retrieve(50.0f, 10.0f);
            std::cout << "Retrieved " << retrieved.size() << " associations from associative memory" << std::endl;
        }
    }
    
    std::cout << "Memory system example completed successfully!" << std::endl;
    return 0;
}