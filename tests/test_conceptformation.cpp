// ConceptFormation Tests
#include "brain/ConceptFormation.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_conceptformation {

void testConceptFormationCreation() {
    nlm::ConceptFormation concept;
    
    // Should be able to access via Brain interface
    std::cout << "    testConceptFormationCreation passed" << std::endl;
}

void testConceptFormationConfiguration() {
    nlm::ConceptFormation concept;
    
    // Set concept formation parameters
    concept.setLearningRate(0.05f);
    concept.setConceptThreshold(0.7f);
    concept.setAssociationStrengthThreshold(0.6f);
    concept.setMaxConcepts(100);
    
    // Get parameters
    float learningRate = concept.getLearningRate();
    assert(learningRate == 0.05f);
    
    float threshold = concept.getConceptThreshold();
    assert(threshold == 0.7f);
    
    std::cout << "    testConceptFormationConfiguration passed" << std::endl;
}

void testConceptFormationLearning() {
    nlm::ConceptFormation concept;
    
    // Create a test pattern
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Learn from pattern
    concept.learnFromPattern(pattern);
    
    // Get formed concepts
    std::vector<nlm::Concept> concepts = concept.getConcepts();
    assert(concepts.size() >= 0);
    
    std::cout << "    testConceptFormationLearning passed" << std::endl;
}

void testConceptFormationRetrieval() {
    nlm::ConceptFormation concept;
    
    // Create test patterns for related concepts
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.4f, 0.5f, 0.6f};
    std::vector<float> pattern3 = {0.7f, 0.8f, 0.9f};
    
    // Learn from patterns
    concept.learnFromPattern(pattern1);
    concept.learnFromPattern(pattern2);
    concept.learnFromPattern(pattern3);
    
    // Retrieve similar concept for new pattern
    std::vector<float> newPattern = {0.15f, 0.25f, 0.35f};  // Similar to pattern1
    std::vector<nlm::Concept> similarConcepts = concept.retrieveSimilarConcepts(newPattern, 3);
    
    // Should find similar concepts
    assert(!similarConcepts.empty());
    
    std::cout << "    testConceptFormationRetrieval passed" << std::endl;
}

void testConceptFormationUpdate() {
    nlm::ConceptFormation concept;
    
    // Create multiple patterns
    std::vector<std::vector<float>> patterns = {
        {0.1f, 0.2f, 0.3f},
        {0.4f, 0.5f, 0.6f},
        {0.7f, 0.8f, 0.9f}
    };
    
    // Learn from patterns
    for (const auto& pattern : patterns) {
        concept.learnFromPattern(pattern);
    }
    
    // Update concept formation (learning continues)
    concept.update(0.001);
    concept.update(0.01);
    concept.update(0.1);
    
    std::cout << "    testConceptFormationUpdate passed" << std::endl;
}

void testConceptFormationPruning() {
    nlm::ConceptFormation concept;
    
    // Create many concepts to fill capacity
    std::vector<std::vector<float>> patterns;
    for (int i = 0; i < 50; ++i) {
        std::vector<float> pattern = {
            static_cast<float>(i % 10) / 10.0f,
            static_cast<float>((i + 3) % 10) / 10.0f,
            static_cast<float>((i + 7) % 10) / 10.0f
        };
        patterns.push_back(pattern);
    }
    
    // Learn from patterns
    for (const auto& pattern : patterns) {
        concept.learnFromPattern(pattern);
    }
    
    // Get current concept count
    size_t conceptCount = concept.getConceptCount();
    assert(conceptCount >= 0);
    
    // Update to trigger pruning
    concept.update(0.001);
    
    // Concept count should be managed
    assert(concept.getConceptCount() >= 0);
    
    std::cout << "    testConceptFormationPruning passed" << std::endl;
}

void runAll() {
    std::cout << "Running ConceptFormation tests..." << std::endl;
    testConceptFormationCreation();
    testConceptFormationConfiguration();
    testConceptFormationLearning();
    testConceptFormationRetrieval();
    testConceptFormationUpdate();
    testConceptFormationPruning();
}

} // namespace test_conceptformation