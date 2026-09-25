// Attention and Concept Formation Tests
// Phase 2: Real Neural Computation Tests
// Comprehensive tests for attention mechanisms and concept formation systems

#include "cognition/ConceptFormation.hpp"
#include "cognition/Attention.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_attention_and_concept {

void testConceptFormation() {
    std::cout << "  Testing concept formation..." << std::endl;
    
    nlm::ConceptFormation concept;
    
    // Test initial state
    size_t initialCount = concept.getConceptCount();
    assert(initialCount == 0);
    
    // Simulate learning some patterns
    std::vector<nlm::NeuronId> pattern1 = {1, 2, 3, 4, 5};
    std::vector<nlm::NeuronId> pattern2 = {2, 3, 4, 5, 6};
    std::vector<nlm::NeuronId> pattern3 = {7, 8, 9, 10, 11};
    
    concept.learnPattern(pattern1);
    concept.learnPattern(pattern2);
    concept.learnPattern(pattern3);
    
    // Check that concepts were learned
    assert(concept.getConceptCount() >= 1);
    
    // Test concept retrieval
    std::vector<nlm::NeuronId> query1 = {1, 2, 3, 4, 5};
    std::vector<nlm::NeuronId> query2 = {2, 3, 4, 5, 6, 7}; // Similar to pattern2
    
    nlm::ConceptId conceptId1 = concept.getConceptForPattern(query1);
    nlm::ConceptId conceptId2 = concept.getConceptForPattern(query2);
    
    // Both queries should match similar patterns (may be same or different concepts)
    assert(conceptId1 != nlm::ConceptId(0) || conceptId2 != nlm::ConceptId(0));
    
    // Test concept activation
    concept.updateConceptActivation(conceptId1, 0.7f);
    float activation = concept.getConceptActivation(conceptId1);
    assert(std::abs(activation - 0.7f) < 0.001f);
    
    // Test concept decay
    concept.updateConceptActivation(conceptId1, 0.0f); // Decay to zero
    activation = concept.getConceptActivation(conceptId1);
    assert(activation == 0.0f);
    
    // Test concept consolidation
    concept.consolidateConcepts();
    
    std::cout << "    PASSED" << std::endl;
}

void testAttentionMechanism() {
    std::cout << "  Testing attention mechanism..." << std::endl;
    
    nlm::Attention attention;
    
    // Test initial attention setup
    assert(attention.getTotalTokens() == 0);
    
    // Simulate tokens for attention
    std::vector<nlm::NeuronId> token1 = {1, 2, 3};
    std::vector<nlm::NeuronId> token2 = {4, 5, 6};
    std::vector<nlm::NeuronId> token3 = {7, 8, 9};
    
    attention.addToken(token1);
    attention.addToken(token2);
    attention.addToken(token3);
    
    assert(attention.getTotalTokens() == 3);
    
    // Simulate input tokens for attention scoring
    std::vector<nlm::NeuronId> query1 = {1, 2, 3, 4};
    std::vector<nlm::NeuronId> query2 = {4, 5, 6, 7};
    
    attention.addInputToken(query1);
    attention.addInputToken(query2);
    
    // Process attention
    attention.processAttention();
    
    // Get attention scores
    std::vector<float> scores = attention.getAttentionScores();
    assert(scores.size() == 2);
    
    // Scores should be between 0 and 1
    for (float score : scores) {
        assert(score >= 0.0f && score <= 1.0f);
    }
    
    // Get selected tokens
    std::vector<nlm::NeuronId> selected = attention.getSelectedTokens(1); // Get top 1
    assert(selected.size() <= 1);
    
    // Test attention reset
    attention.reset();
    assert(attention.getTotalTokens() == 0);
    
    std::cout << "    PASSED" << std::endl;
}

void testConceptFormationIntegration() {
    std::cout << "  Testing concept formation integration..." << std::endl;
    
    // This test would require integration with a full brain simulation
    // For now, test the concept formation system independently
    
    nlm::ConceptFormation concept;
    
    // Create multiple patterns to simulate learning
    std::vector<std::vector<nlm::NeuronId>> patterns;
    
    for (int i = 0; i < 10; ++i) {
        std::vector<nlm::NeuronId> pattern;
        for (int j = 0; j < 5; ++j) {
            pattern.push_back(i * 10 + j);
        }
        patterns.push_back(pattern);
    }
    
    // Learn all patterns
    for (const auto& pattern : patterns) {
        concept.learnPattern(pattern);
    }
    
    // Check concept count
    assert(concept.getConceptCount() > 0);
    
    // Test concept retrieval with new pattern
    std::vector<nlm::NeuronId> newPattern = {100, 101, 102, 103, 104};
    nlm::ConceptId conceptId = concept.getConceptForPattern(newPattern);
    
    // Should either find matching concept or create new one
    assert(conceptId != nlm::ConceptId(0));
    
    // Test concept activation update
    concept.updateConceptActivation(conceptId, 0.5f);
    float activation = concept.getConceptActivation(conceptId);
    assert(std::abs(activation - 0.5f) < 0.001f);
    
    // Test concept consolidation
    concept.consolidateConcepts();
    
    std::cout << "    PASSED" << std::endl;
}

void testAttentionConceptIntegration() {
    std::cout << "  Testing attention and concept integration..." << std::endl;
    
    nlm::ConceptFormation concept;
    nlm::Attention attention;
    
    // Create some concepts
    std::vector<nlm::NeuronId> pattern1 = {1, 2, 3, 4, 5};
    std::vector<nlm::NeuronId> pattern2 = {6, 7, 8, 9, 10};
    
    concept.learnPattern(pattern1);
    concept.learnPattern(pattern2);
    
    // Get concept IDs
    nlm::ConceptId conceptId1 = concept.getConceptForPattern(pattern1);
    nlm::ConceptId conceptId2 = concept.getConceptForPattern(pattern2);
    
    // Create tokens from concepts
    std::vector<nlm::NeuronId> token1 = {1, 2, 3};
    std::vector<nlm::NeuronId> token2 = {6, 7, 8};
    
    attention.addToken(token1);
    attention.addToken(token2);
    
    // Add input query
    std::vector<nlm::NeuronId> query = {1, 2, 3, 6};
    attention.addInputToken(query);
    
    // Process attention
    attention.processAttention();
    
    // Both tokens should have attention scores
    std::vector<float> scores = attention.getAttentionScores();
    assert(scores.size() == 2);
    
    // Concept activations should be updated based on attention
    concept.updateConceptActivation(conceptId1, 0.8f);
    concept.updateConceptActivation(conceptId2, 0.2f);
    
    float activation1 = concept.getConceptActivation(conceptId1);
    float activation2 = concept.getConceptActivation(conceptId2);
    
    assert(std::abs(activation1 - 0.8f) < 0.001f);
    assert(std::abs(activation2 - 0.2f) < 0.001f);
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Attention and Concept Formation Tests ===" << std::endl;
    
    testConceptFormation();
    testAttentionMechanism();
    testConceptFormationIntegration();
    testAttentionConceptIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Attention and Concept Tests PASSED ===" << std::endl;
}

} // namespace test_attention_and_concept
