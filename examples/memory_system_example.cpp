// NLM Memory System Example: Working with Different Memory Types

#include "nlm/brain/Brain.hpp"
#include "nlm/config/Config.hpp"
#include <iostream>
#include <string>

void demonstrateWorkingMemory(nlm::Brain& brain) {
    std::cout << "\n=== Working Memory Example ===" << std::endl;
    
    auto* workingMem = brain.getWorkingMemory();
    if (!workingMem) {
        std::cout << "Working memory not available!" << std::endl;
        return;
    }
    
    // Configure working memory
    // workingMem->setCapacity(100); // If API available
    workingMem->setDecayRate(0.02f);  // 2% decay per step
    
    // Store some data
    workingMem->store(1, 0.9f);  // Task 1 with high priority
    workingMem->store(2, 0.5f);  // Task 2 with medium priority
    workingMem->store(3, 0.2f);  // Task 3 with low priority
    
    std::cout << "Stored data in working memory:" << std::endl;
    std::cout << "  Task 1: " << workingMem->retrieve(1) << std::endl;
    std::cout << "  Task 2: " << workingMem->retrieve(2) << std::endl;
    std::cout << "  Task 3: " << workingMem->retrieve(3) << std::endl;
    
    // Perform operations on stored data
    float processedValue = workingMem->retrieve(1) * 2.0f;
    workingMem->store(1, processedValue);
    std::cout << "After processing: Task 1 = " << workingMem->retrieve(1) << std::endl;
    
    // Show memory usage
    size_t currentSize = workingMem->getCurrentSize();
    size_t capacity = workingMem->getCapacity();
    std::cout << "Memory usage: " << currentSize << "/" << capacity << std::endl;
    
    // Demonstrate decay
    workingMem->decay(0.1f);  // 10% decay
    std::cout << "After decay - Task 1: " << workingMem->retrieve(1) << std::endl;
}

void demonstrateEpisodicMemory(nlm::Brain& brain) {
    std::cout << "\n=== Episodic Memory Example ===" << std::endl;
    
    auto* episodicMem = brain.getEpisodicMemory();
    if (!episodicMem) {
        std::cout << "Episodic memory not available!" << std::endl;
        return;
    }
    
    // Create and store an episodic memory item
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.metadata = "Visited laboratory - completed experiment";
    
    // Store neural pattern representing this experience
    episode.neurons.push_back(100);
    episode.values.push_back(0.9f);
    episode.neurons.push_back(101);
    episode.values.push_back(0.8f);
    episode.neurons.push_back(102);
    episode.values.push_back(0.7f);
    
    episodicMem->storeEpisode(episode);
    std::cout << "Stored episodic memory: " << episode.metadata << std::endl;
    
    // Retrieve the episode
    nlm::EpisodicMemoryItem retrieved = episodicMem->retrieveEpisode(0);
    std::cout << "Retrieved episodic memory: " << retrieved.metadata << std::endl;
    std::cout << "Pattern: [";
    for (float val : retrieved.values) {
        std::cout << val << " ";
    }
    std::cout << "]" << std::endl;
    
    // Get recent episodes
    auto recentEpisodes = episodicMem->getRecentEpisodes(3);
    std::cout << "Recent episodes count: " << recentEpisodes.size() << std::endl;
    
    // Store another episode
    nlm::EpisodicMemoryItem meeting;
    meeting.timestamp = 1;
    meeting.metadata = "Team meeting - discussed next project";
    meeting.neurons.push_back(200);
    meeting.values.push_back(0.85f);
    meeting.neurons.push_back(201);
    meeting.values.push_back(0.75f);
    
    episodicMem->storeEpisode(meeting);
    std::cout << "Stored additional episode: " << meeting.metadata << std::endl;
    
    // Check episode count
    size_t episodeCount = episodicMem->getEpisodeCount();
    std::cout << "Total episodes stored: " << episodeCount << std::endl;
}

void demonstrateSemanticMemory(nlm::Brain& brain) {
    std::cout << "\n=== Semantic Memory Example ===" << std::endl;
    
    auto* semanticMem = brain.getSemanticMemory();
    if (!semanticMem) {
        std::cout << "Semantic memory not available!" << std::endl;
        return;
    }
    
    // Store semantic facts
    semanticMem->storeFact("color_red", "#FF0000");
    semanticMem->storeFact("color_blue", "#0000FF");
    semanticMem->storeFact("gravity", "9.8 m/s²");
    semanticMem->storeFact("water_boiling_point", "100°C");
    
    std::cout << "Stored semantic facts:" << std::endl;
    auto allFacts = semanticMem->getAllFacts();
    for (const auto& fact : allFacts) {
        std::cout << "  " << fact.first << " = " << fact.second << std::endl;
    }
    
    // Retrieve facts
    std::string redColor = semanticMem->retrieveFact("color_red");
    std::string gravity = semanticMem->retrieveFact("gravity");
    std::cout << "\nRetrieved facts:" << std::endl;
    std::cout << "  Red color: " << redColor << std::endl;
    std::cout << "  Gravity: " << gravity << std::endl;
    
    // Check if facts exist
    if (semanticMem->hasFact("color_blue")) {
        std::cout << "Blue fact is available in semantic memory" << std::endl;
    }
}

void demonstrateProceduralMemory(nlm::Brain& brain) {
    std::cout << "\n=== Procedural Memory Example ===" << std::endl;
    
    auto* proceduralMem = brain.getProceduralMemory();
    if (!proceduralMem) {
        std::cout << "Procedural memory not available!" << std::endl;
        return;
    }
    
    // Create a motor skill pattern
    std::vector<nlm::NeuronId> reachingPattern;
    for (int i = 0; i < 5; ++i) {
        reachingPattern.push_back(static_cast<nlm::NeuronId>(300 + i));
    }
    
    // Learn the skill
    proceduralMem->learnSkill("reach_right", reachingPattern);
    std::cout << "Learned motor skill: reach_right" << std::endl;
    
    // Create another skill
    std::vector<nlm::NeuronId> walkingPattern;
    for (int i = 0; i < 8; ++i) {
        walkingPattern.push_back(static_cast<nlm::NeuronId>(400 + i));
    }
    
    proceduralMem->learnSkill("walk_forward", walkingPattern);
    std::cout << "Learned motor skill: walk_forward" << std::endl;
    
    // Get skill information
    nlm::Skill* reachingSkill = proceduralMem->getSkill("reach_right");
    if (reachingSkill) {
        std::cout << "Skill details:" << std::endl;
        std::cout << "  Name: " << reachingSkill->name << std::endl;
        std::cout << "  Pattern size: " << reachingSkill->neuralPattern.size() << std::endl;
        std::cout << "  Proficiency: " << reachingSkill->proficiency << std::endl;
    }
    
    // Update proficiency
    proceduralMem->updateProficiency("reach_right", 0.1f);
    std::cout << "After practice - reach_right proficiency: " << 
        (reachingSkill ? reachingSkill->proficiency : 0.0f) << std::endl;
}

void demonstrateAssociativeMemory(nlm::Brain& brain) {
    std::cout << "\n=== Associative Memory Example ===" << std::endl;
    
    auto* associativeMem = brain.getAssociativeMemory();
    if (!associativeMem) {
        std::cout << "Associative memory not available!" << std::endl;
        return;
    }
    
    // Create associations
    associativeMem->associate(10, 20, 0.9f);  // Strong association
    associativeMem->associate(10, 21, 0.6f);  // Medium association
    associativeMem->associate(11, 21, 0.8f);  // Another medium association
    
    std::cout << "Created associations:" << std::endl;
    std::cout << "  10 -> 20 (strength: 0.9)" << std::endl;
    std::cout << "  10 -> 21 (strength: 0.6)" << std::endl;
    std::cout << "  11 -> 21 (strength: 0.8)" << std::endl;
    
    // Get associated neurons
    std::vector<nlm::NeuronId> assoc10 = associativeMem->getAssociations(10);
    std::cout << "Neurons associated with 10: ";
    for (nlm::NeuronId id : assoc10) {
        std::cout << id << " ";
    }
    std::cout << std::endl;
    
    // Get association strength
    float strength = associativeMem->getAssociationStrength(10, 20);
    std::cout << "Association strength between 10 and 20: " << strength << std::endl;
    
    // Update association strength
    associativeMem->updateAssociation(10, 20, 0.2f);  // Strengthen
    float newStrength = associativeMem->getAssociationStrength(10, 20);
    std::cout << "After strengthening: " << newStrength << std::endl;
    
    // Create complex associations
    // A -> B -> C chain
    associativeMem->associate(1, 2, 0.95f);  // Very strong A-B
    associativeMem->associate(2, 3, 0.9f);   // Very strong B-C
    
    // This could enable transitive inference: 1 -> 3
    std::cout << "Created transitive association chain: 1 -> 2 -> 3" << std::endl;
}

int main() {
    std::cout << "=== NLM Memory Systems Example ===" << std::endl;
    
    // Create brain with memory support
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::User);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::User);
    
    // Configure memory systems
    config->set("working_memory_capacity", 200, nlm::ConfigSource::User);
    config->set("episodic_memory_capacity", 1000, nlm::ConfigSource::User);
    config->set("semantic_memory_capacity", 5000, nlm::ConfigSource::User);
    config->set("procedural_memory_capacity", 300, nlm::ConfigSource::User);
    config->set("associative_memory_capacity", 1000, nlm::ConfigSource::User);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Demonstrate each memory system
    demonstrateWorkingMemory(brain);
    demonstrateEpisodicMemory(brain);
    demonstrateSemanticMemory(brain);
    demonstrateProceduralMemory(brain);
    demonstrateAssociativeMemory(brain);
    
    std::cout << "\n=== Memory Systems Example Completed ===" << std::endl;
    
    return 0;
}
