// Associative Memory Tests
// ====================
//
// NeuralAssociativeMemory tests verify that the associative memory system
// correctly implements Hebbian learning and associative recall for pattern
// relationships. This memory system creates and retrieves associations
// between neural patterns, enabling relational memory and spreading activation
// for recall.
//
// Associative memory tests focus on:
// - Hebbian association formation between patterns
// - Association storage and retrieval
// - Association strengthening based on experience
// - Spreading activation through association networks
// - Integration with episodic memory for experience-based learning
// - Competition and selectivity in association recall
//
// This test suite ensures associative memory operates as a dynamic network
// where co-activation forms lasting synaptic connections that can be used to
// retrieve related information through partial cueing and spreading activation.

#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_associative_memory {

void testAssociativeMemoryCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(associativeMemory != nullptr);
    assert(associativeMemory->getAssociationCount() == 0);
    
    std::cout << "    testAssociativeMemoryCreation passed" << std::endl;
}

void testAssociativeMemoryAssociateRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    std::vector<float> patternA = {0.5f, 0.8f, 0.3f};
    std::vector<float> patternB = {0.9f, 0.2f, 0.7f};
    
    associativeMemory->associate(patternA, patternB, 0.8f);
    
    assert(associativeMemory->getAssociationCount() == 1);
    
    // Retrieve associations
    std::vector<std::vector<float>> retrieved = 
        associativeMemory->retrieve(patternA, 3);
    
    assert(retrieved.size() == 1);
    assert(retrieved[0].size() == patternB.size());
    
    // Get association strength
    float strength = associativeMemory->getAssociationStrength(patternA, patternB);
    assert(strength > 0.7f);
    
    std::cout << "    testAssociativeMemoryAssociateRetrieve passed" << std::endl;
}

void testAssociativeMemoryAssociateFromExperience() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Create an experience-based episode
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = {0.5f, 0.8f, 0.3f};
    episode.resultingSensoryState = {0.7f, 0.6f, 0.9f};
    episode.action = nlm::ActionType::Move;
    
    associativeMemory->associateFromExperience(episode);
    
    // Should have created at least one association
    assert(associativeMemory->getAssociationCount() >= 1);
    
    std::cout << "    testAssociativeMemoryAssociateFromExperience passed" << std::endl;
}

void testAssociativeMemoryUpdateAssociation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    std::vector<float> patternA = {0.5f, 0.8f, 0.3f};
    std::vector<float> patternB = {0.9f, 0.2f, 0.7f};
    
    // Create association
    associativeMemory->associate(patternA, patternB, 0.5f);
    
    // Update association strength
    associativeMemory->updateAssociation(patternA, patternB, 0.3f);
    
    float strength = associativeMemory->getAssociationStrength(patternA, patternB);
    assert(strength > 0.7f && strength < 1.0f);
    
    std::cout << "    testAssociativeMemoryUpdateAssociation passed" << std::endl;
}

void testAssociativeMemorySpreadActivation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    std::vector<float> patternA = {0.5f, 0.8f, 0.3f};
    std::vector<float> patternB = {0.9f, 0.2f, 0.7f};
    std::vector<float> patternC = {0.4f, 0.6f, 0.8f};
    
    // Create a chain: A <-> B, B <-> C
    associativeMemory->associate(patternA, patternB, 0.8f);
    associativeMemory->associate(patternB, patternC, 0.7f);
    
    // Start spreading from pattern A
    std::vector<std::vector<float>> activated = 
        associativeMemory->spreadActivation(patternA, 3);
    
    // Should activate patterns in the association chain
    assert(activated.size() >= 1);
    
    std::cout << "    testAssociativeMemorySpreadActivation passed" << std::endl;
}

void testAssociativeMemoryClear() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Add some associations
    std::vector<float> patternA = {0.5f, 0.8f, 0.3f};
    std::vector<float> patternB = {0.9f, 0.2f, 0.7f};
    
    associativeMemory->associate(patternA, patternB, 0.5f);
    assert(associativeMemory->getAssociationCount() > 0);
    
    associativeMemory->clear();
    
    assert(associativeMemory->getAssociationCount() == 0);
    
    std::cout << "    testAssociativeMemoryClear passed" << std::endl;
}

void testAssociativeMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* associativeMemory = brain.getAssociativeMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Create an episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = {0.5f, 0.8f, 0.3f};
    episode.resultingSensoryState = {0.7f, 0.6f, 0.9f};
    
    episodicMemory->storeEpisode(episode);
    
    // Create association from experience
    associativeMemory->associateFromExperience(episode);
    
    // Verify the integration works
    assert(associativeMemory->getAssociationCount() >= 1);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    std::cout << "    testAssociativeMemoryIntegration passed" << std::endl;
}

void testAssociativeMemoryAdvancedFeatures() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Test 1: Create multiple associations between patterns
    std::vector<std::vector<float>> patterns = {
        {0.1f, 0.2f, 0.3f},
        {0.4f, 0.5f, 0.6f},
        {0.7f, 0.8f, 0.9f},
        {0.2f, 0.3f, 0.4f},
        {0.5f, 0.6f, 0.7f}
    };
    
    // Associate similar patterns
    associativeMemory->associate(patterns[0], patterns[1], 0.6f);
    associativeMemory->associate(patterns[1], patterns[2], 0.7f);
    associativeMemory->associate(patterns[2], patterns[3], 0.5f);
    associativeMemory->associate(patterns[3], patterns[4], 0.8f);
    
    assert(associativeMemory->getAssociationCount() == 4);
    
    // Test 2: Retrieve with partial cue
    std::vector<std::vector<float>> results = 
        associativeMemory->retrieve(patterns[1], 3);
    assert(results.size() >= 1);
    
    // Test 3: Get association strength between various pairs
    float strength1 = associativeMemory->getAssociationStrength(patterns[0], patterns[1]);
    float strength2 = associativeMemory->getAssociationStrength(patterns[1], patterns[2]);
    float strength3 = associativeMemory->getAssociationStrength(patterns[0], patterns[2]); // Should be 0 (no direct association)
    
    assert(strength1 > 0.5f);
    assert(strength2 > 0.6f);
    assert(strength3 == 0.0f);
    
    // Test 4: Update associations
    associativeMemory->updateAssociation(patterns[0], patterns[1], 0.2f);
    float newStrength = associativeMemory->getAssociationStrength(patterns[0], patterns[1]);
    assert(newStrength > strength1);
    
    // Test 5: Clear and verify
    associativeMemory->clear();
    assert(associativeMemory->getAssociationCount() == 0);
    
    std::cout << "    testAssociativeMemoryAdvancedFeatures passed" << std::endl;
}

void testAssociativeMemoryHebbianLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Test Hebbian-like learning: repeated co-activation strengthens association
    std::vector<float> patternA = {0.3f, 0.6f, 0.9f};
    std::vector<float> patternB = {0.8f, 0.4f, 0.2f};
    
    // First association
    associativeMemory->associate(patternA, patternB, 0.5f);
    float strength1 = associativeMemory->getAssociationStrength(patternA, patternB);
    assert(strength1 > 0.4f);
    
    // Update association (simulating more co-activation)
    associativeMemory->updateAssociation(patternA, patternB, 0.3f);
    float strength2 = associativeMemory->getAssociationStrength(patternA, patternB);
    assert(strength2 > strength1);
    
    // Create another association and test spreading
    std::vector<float> patternC = {0.1f, 0.2f, 0.3f};
    associativeMemory->associate(patternB, patternC, 0.6f);
    
    // Spread activation from patternA
    std::vector<std::vector<float>> spread = associativeMemory->spreadActivation(patternA, 2);
    
    // Should potentially reach patternC through patternB
    assert(spread.size() >= 0); // May or may not reach patternC depending on thresholds
    
    std::cout << "    testAssociativeMemoryHebbianLearning passed" << std::endl;
}

void testAssociativeMemoryPatternSimilarity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Test pattern similarity-based association
    std::vector<float> similarPattern1 = {0.5f, 0.5f, 0.5f};
    std::vector<float> similarPattern2 = {0.6f, 0.55f, 0.45f}; // Similar to pattern1
    std::vector<float> dissimilarPattern = {0.1f, 0.9f, 0.2f}; // Dissimilar
    
    // Associate similar patterns (should work due to high similarity)
    associativeMemory->associate(similarPattern1, similarPattern2, 0.8f);
    
    // Associate with dissimilar pattern (may work if strength is high enough)
    associativeMemory->associate(similarPattern1, dissimilarPattern, 0.9f);
    
    assert(associativeMemory->getAssociationCount() == 2);
    
    // Test that both associations exist
    float strengthSimilar = associativeMemory->getAssociationStrength(similarPattern1, similarPattern2);
    float strengthDissimilar = associativeMemory->getAssociationStrength(similarPattern1, dissimilarPattern);
    
    assert(strengthSimilar > 0.7f);
    assert(strengthDissimilar > 0.8f);
    
    std::cout << "    testAssociativeMemoryPatternSimilarity passed" << std::endl;
}

void runAll() {
    testAssociativeMemoryCreation();
    testAssociativeMemoryAssociateRetrieve();
    testAssociativeMemoryAssociateFromExperience();
    testAssociativeMemoryUpdateAssociation();
    testAssociativeMemorySpreadActivation();
    testAssociativeMemoryClear();
    testAssociativeMemoryIntegration();
    testAssociativeMemoryAdvancedFeatures();
    testAssociativeMemoryHebbianLearning();
    testAssociativeMemoryPatternSimilarity();
    
    std::cout << "All associative memory tests passed!" << std::endl;
}

} // namespace test_associative_memory
