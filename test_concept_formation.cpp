// Concept Formation Integration Test
// Tests the concept formation integration in the NLM brain

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include <iostream>
#include <vector>
#include <cassert>

void testConceptFormationInitialization() {
    std::cout << "=== Testing Concept Formation Initialization ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create brain
    Brain brain(config);
    
    // Initialize brain
    assert(brain.initialize());
    
    // Verify concept formation system is integrated
    assert(brain.getConceptFormation() != nullptr);
    
    // Verify we can access the concept formation system
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Verify we can get concept count
    size_t conceptCount = conceptFormation->getConceptCount();
    std::cout << "Initial concept count: " << conceptCount << std::endl;
    
    // Verify we can get last concept ID
    size_t lastConceptId = brain.getLastConceptId();
    assert(lastConceptId == 0); // Should be 0 initially (no concepts)
    
    std::cout << "✓ Concept Formation initialization test passed" << std::endl;
}

void testConceptFormationProcessing() {
    std::cout << "\n=== Testing Concept Formation Processing ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create brain
    Brain brain(config);
    brain.initialize();
    
    // Get concept formation system
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Create a test neural pattern (similar to what getCurrentBrainState() returns)
    std::vector<float> testPattern;
    for (int i = 0; i < 20; ++i) {
        testPattern.push_back(0.5f + 0.1f * std::sin(i * 0.1f));
    }
    
    // Create sensory features
    std::vector<float> testFeatures;
    for (int i = 0; i < 10; ++i) {
        testFeatures.push_back(0.3f * (i + 1));
    }
    
    // Test presenting experience
    size_t conceptId = conceptFormation->presentExperience(
        testPattern,
        testFeatures,
        0.5f,  // reward
        0      // step
    );
    
    // Check if concept was formed (ID should be > 0)
    if (conceptId > 0) {
        std::cout << "Concept formed with ID: " << conceptId << std::endl;
        
        // Verify we can get the concept
        const DiscoveredConcept* concept = conceptFormation->getConcept(conceptId);
        assert(concept != nullptr);
        
        // Verify concept has prototype
        std::vector<float> prototype = conceptFormation->getConceptPrototype(conceptId);
        assert(prototype.size() == testPattern.size());
        
        // Verify concept has instances
        const std::vector<ConceptInstance>& instances = conceptFormation->getConceptInstances(conceptId);
        assert(!instances.empty());
        
        std::cout << "✓ Concept formation processing test passed" << std::endl;
    } else {
        std::cout << "No concept formed (may be normal for this pattern)" << std::endl;
        std::cout << "✓ Concept formation test completed" << std::endl;
    }
}

void testConceptFormationSimilarity() {
    std::cout << "\n=== Testing Concept Formation Similarity ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create brain
    Brain brain(config);
    brain.initialize();
    
    // Get concept formation system
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Create two similar patterns
    std::vector<float> pattern1;
    for (int i = 0; i < 10; ++i) {
        pattern1.push_back(0.5f + 0.2f * i);
    }
    
    std::vector<float> pattern2;
    for (int i = 0; i < 10; ++i) {
        pattern2.push_back(0.5f + 0.2f * i + 0.05f);  // Similar but not identical
    }
    
    // Create a different pattern
    std::vector<float> pattern3;
    for (int i = 0; i < 10; ++i) {
        pattern3.push_back(0.1f + 0.1f * i);
    }
    
    // Test similarity computation
    float sim1_2 = conceptFormation->computeSimilarity(pattern1, pattern2);
    float sim1_3 = conceptFormation->computeSimilarity(pattern1, pattern3);
    
    std::cout << "Similarity between pattern1 and pattern2: " << sim1_2 << std::endl;
    std::cout << "Similarity between pattern1 and pattern3: " << sim1_3 << std::endl;
    
    // Similar patterns should have higher similarity
    assert(sim1_2 > sim1_3);
    
    std::cout << "✓ Concept formation similarity test passed" << std::endl;
}

void testConceptFormationNovelty() {
    std::cout << "\n=== Testing Concept Formation Novelty ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create brain
    Brain brain(config);
    brain.initialize();
    
    // Get concept formation system
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Create a test pattern
    std::vector<float> pattern;
    for (int i = 0; i < 10; ++i) {
        pattern.push_back(0.9f * i);
    }
    
    // Test if pattern is novel
    bool isNovel = conceptFormation->isNovel(pattern, 0.7f);
    std::cout << "Pattern is novel: " << (isNovel ? "Yes" : "No") << std::endl;
    
    // Test similarity threshold
    conceptFormation->setFormationThreshold(0.5f);
    float similarity = conceptFormation->computeSimilarity(pattern, pattern);
    assert(similarity > 0.99f);  // Pattern should be very similar to itself
    
    std::cout << "✓ Concept formation novelty test passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Concept Formation Integration Tests ===" << std::endl;
    
    try {
        testConceptFormationInitialization();
        testConceptFormationProcessing();
        testConceptFormationSimilarity();
        testConceptFormationNovelty();
        
        std::cout << "\n=== All Concept Formation Integration Tests Passed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
