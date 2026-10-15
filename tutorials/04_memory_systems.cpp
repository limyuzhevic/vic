// Tutorial 4: Building Custom Memory Systems
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/NeuralAssociativeMemory.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "Tutorial 4: Building Custom Memory Systems" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "✗ Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Brain initialized successfully!" << std::endl;
    
    // Access memory systems
    auto* extensibleBrain = dynamic_cast<nlm::ExtensibleBrain*>(&brain);
    if (extensibleBrain) {
        // Access working memory
        auto* workingMemory = extensibleBrain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "✓ Working memory system available" << std::endl;
            std::cout << "  Capacity: " << workingMemory->getCapacity() << std::endl;
            std::cout << "  Active traces: " << workingMemory->getActiveTraces() << std::endl;
            
            // Store data in working memory
            for (int i = 0; i < 10; ++i) {
                workingMemory->store(static_cast<float>(i), static_cast<float>(i));
            }
            
            std::cout << "✓ Stored 10 traces in working memory" << std::endl;
            
            // Retrieve data from working memory
            float retrieved = workingMemory->retrieve(5.0f);
            std::cout << "✓ Retrieved from working memory: " << retrieved << std::endl;
        }
        
        // Access episodic memory
        auto* episodicMemory = extensibleBrain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "✓ Episodic memory system available" << std::endl;
            std::cout << "  Max episodes: " << episodicMemory->getMaxEpisodes() << std::endl;
            std::cout << "  Current episodes: " << episodicMemory->getEpisodeCount() << std::endl;
            
            // Store data in episodic memory
            for (int i = 0; i < 20; ++i) {
                episodicMemory->store(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
            }
            
            std::cout << "✓ Stored 20 episodes in episodic memory" << std::endl;
            
            // Retrieve data from episodic memory
            float retrieved = episodicMemory->retrieve(10.0f, 5.0f);
            std::cout << "✓ Retrieved from episodic memory: " << retrieved << std::endl;
        }
        
        // Access associative memory
        auto* associativeMemory = extensibleBrain->getAssociativeMemory();
        if (associativeMemory) {
            std::cout << "✓ Associative memory system available" << std::endl;
            std::cout << "  Max associations: " << associativeMemory->getMaxAssociations() << std::endl;
            std::cout << "  Current associations: " << associativeMemory->getAssociationCount() << std::endl;
            
            // Store associations in associative memory
            for (int i = 0; i < 30; ++i) {
                associativeMemory->store(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i) * 0.1f);
            }
            
            std::cout << "✓ Stored 30 associations in associative memory" << std::endl;
            
            // Retrieve associations from associative memory
            std::vector<float> retrieved = associativeMemory->retrieve(15.0f, 10.0f);
            std::cout << "✓ Retrieved " << retrieved.size() << " associations from associative memory" << std::endl;
        }
    }
    
    std::cout << "✓ Custom memory system tutorial completed successfully!" << std::endl;
    return 0;
}