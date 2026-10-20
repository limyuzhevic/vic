// NeuralAssociativeMemory Tests
#include "memory/NeuralAssociativeMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neural_associative_memory {

void testMemoryCreation() {
    nlm::NeuralAssociativeMemory memory;
    
    assert(memory.getAssociationCount() == 0);
    
    std::cout << "    testMemoryCreation passed" << std::endl;
}

void testMemoryInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    assert(memory.getAssociationCount() == 0);
    
    std::cout << "    testMemoryInitialization passed" << std::endl;
}

void testAssociationCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create two patterns
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    
    // Create association
    memory.associate(patternA, patternB, 0.8f);
    
    assert(memory.getAssociationCount() == 1);
    
    // Get association strength
    float strength = memory.getAssociationStrength(patternA, patternB);
    assert(strength == 0.8f);
    
    std::cout << "    testAssociationCreation passed" << std::endl;
}

void testAssociationFromExperience() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create an episode from experience
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 10.0f;
    
    // Create second pattern from resulting state
    std::vector<float> resultingPattern = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    
    // Associate from experience
    memory.associateFromExperience(episode);
    
    // Should have created at least one association
    assert(memory.getAssociationCount() >= 1);
    
    std::cout << "    testAssociationFromExperience passed" << std::endl;
}

void testAssociationRetrieval() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create pattern associations
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    std::vector<float> patternC = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};  // Similar to A
    
    memory.associate(patternA, patternB, 0.9f);
    memory.associate(patternA, patternC, 0.7f);
    
    // Retrieve associations for patternA
    auto retrieved = memory.retrieve(patternA, 5);
    
    // Should find at least patternB and patternC
    assert(!retrieved.empty());
    assert(retrieved.size() >= 2);
    
    std::cout << "    testAssociationRetrieval passed" << std::endl;
}

void testAssociationStrength() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create patterns
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    std::vector<float> patternC = {0.7f, 0.8f, 0.9f, 1.0f, 1.1f};
    
    // Create initial associations with different strengths
    memory.associate(patternA, patternB, 0.5f);
    memory.associate(patternA, patternC, 0.8f);
    
    // Check strengths
    float strengthAB = memory.getAssociationStrength(patternA, patternB);
    float strengthAC = memory.getAssociationStrength(patternA, patternC);
    
    assert(strengthAB == 0.5f);
    assert(strengthAC == 0.8f);
    
    // Update one association
    memory.updateAssociation(patternA, patternB, 0.3f);  // Increase to 0.8f
    
    float newStrengthAB = memory.getAssociationStrength(patternA, patternB);
    assert(newStrengthAB == 0.8f);
    
    std::cout << "    testAssociationStrength passed" << std::endl;
}

void testAssociationSpreadActivation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create a chain of associations: A -> B -> C -> D
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
    std::vector<float> patternC = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    std::vector<float> patternD = {0.7f, 0.8f, 0.9f, 1.0f, 1.1f};
    
    memory.associate(patternA, patternB, 0.9f);
    memory.associate(patternB, patternC, 0.8f);
    memory.associate(patternC, patternD, 0.7f);
    
    // Spread activation from patternA
    auto spreadResults = memory.spreadActivation(patternA, 3);
    
    // Should activate B, C, and D
    assert(!spreadResults.empty());
    
    std::cout << "    testAssociationSpreadActivation passed" << std::endl;
}

void testMemoryClear() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Create some associations
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    
    memory.associate(patternA, patternB, 0.9f);
    
    assert(memory.getAssociationCount() == 1);
    
    // Clear memory
    memory.clear();
    
    assert(memory.getAssociationCount() == 0);
    
    std::cout << "    testMemoryClear passed" << std::endl;
}

void testAssociationSimilarity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralAssociativeMemory memory;
    memory.initialize(&brain);
    
    // Test similarity computation
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};  // Similar to A
    std::vector<float> patternC = {0.8f, 0.9f, 1.0f, 1.1f, 1.2f};  // Very different from A
    
    float similarityAB = memory.computeSimilarity(patternA, patternB);
    float similarityAC = memory.computeSimilarity(patternA, patternC);
    
    // Pattern A and B should be more similar than A and C
    assert(similarityAB > similarityAC);
    
    std::cout << "    testAssociationSimilarity passed" << std::endl;
}

void runAll() {
    testMemoryCreation();
    testMemoryInitialization();
    testAssociationCreation();
    testAssociationFromExperience();
    testAssociationRetrieval();
    testAssociationStrength();
    testAssociationSpreadActivation();
    testMemoryClear();
    testAssociationSimilarity();
}

} // namespace test_neural_associative_memory
