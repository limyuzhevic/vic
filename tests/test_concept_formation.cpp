// Concept Formation Tests
// Tests concept formation mechanisms and attention integration

#include "cognition/ConceptFormation.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_concept_formation {

void testConceptCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::ConceptFormation conceptForm;
    conceptForm.initialize(&brain);
    conceptForm.setFormationThreshold(0.7f);
    
    // Present first experience
    std::vector<float> features1{0.8f, 0.9f, 0.7f, 0.6f};
    size_t conceptId1 = conceptForm.presentExperience(
        features1, features1, 0.8f, 0
    );
    
    assert(conceptId1 > 0); // New concept created
    
    // Present similar experience (should match existing concept)
    std::vector<float> features2{0.85f, 0.92f, 0.68f, 0.62f};
    size_t conceptId2 = conceptForm.presentExperience(
        features2, features2, 0.7f, 1
    );
    
    assert(conceptId2 == conceptId1); // Same concept
    
    std::cout << "    testConceptCreation passed" << std::endl;
}

void testPrototypeEvolution() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::ConceptFormation conceptForm;
    conceptForm.initialize(&brain);
    
    // Create concept with initial pattern
    std::vector<float> initialPattern{0.5f, 0.5f, 0.5f};
    size_t conceptId = conceptForm.presentExperience(
        initialPattern, initialPattern, 0.5f, 0
    );
    
    auto prototype = conceptForm.getConceptPrototype(conceptId);
    assert(prototype.size() == initialPattern.size());
    
    // Update with similar but different pattern
    std::vector<float> updatedPattern{0.6f, 0.4f, 0.7f};
    conceptForm.updateConcept(conceptId, updatedPattern, updatedPattern, 0.8f);
    
    auto newPrototype = conceptForm.getConceptPrototype(conceptId);
    for (size_t i = 0; i < newPrototype.size(); ++i) {
        assert(newPrototype[i] >= 0.0f && newPrototype[i] <= 1.0f);
    }
    
    // Check stability
    float stability = conceptForm.getConceptStability(conceptId);
    assert(stability >= 0.0f && stability <= 1.0f);
    
    std::cout << "    testPrototypeEvolution passed" << std::endl;
}

void testConceptBasedOnAttention() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(220), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::ConceptFormation conceptForm;
    conceptForm.initialize(&brain);
    
    // Create multiple concepts
    std::vector<std::vector<float>> patterns = {
        {0.9f, 0.8f, 0.7f},
        {0.3f, 0.4f, 0.5f},
        {0.6f, 0.7f, 0.8f},
        {0.1f, 0.2f, 0.3f}
    };
    
    std::vector<size_t> conceptIds;
    for (size_t i = 0; i < patterns.size(); ++i) {
        size_t conceptId = conceptForm.presentExperience(
            patterns[i], patterns[i], 0.6f + (i * 0.1f), i
        );
        conceptIds.push_back(conceptId);
    }
    
    // Store concepts in working memory
    for (size_t conceptId : conceptIds) {
        conceptForm.storeConceptInWorkingMemory(conceptId);
    }
    
    // Verify concept selection based on attention
    std::vector<size_t> conceptsInMemory = conceptForm.getConceptsInWorkingMemory();
    assert(!conceptsInMemory.empty());
    
    size_t selectedConcept = conceptForm.getConceptBasedOnAttention();
    assert(selectedConcept > 0);
    
    // Verify selected concept is in working memory
    assert(std::find(conceptsInMemory.begin(), conceptsInMemory.end(), selectedConcept) != conceptsInMemory.end());
    
    std::cout << "    testConceptBasedOnAttention passed" << std::endl;
}

void testConceptIntegrationWithAttention() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(240), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::ConceptFormation conceptForm;
    conceptForm.initialize(&brain);
    
    // Create concept and apply attentional selection
    std::vector<float> pattern{0.7f, 0.8f, 0.9f};
    size_t conceptId = conceptForm.presentExperience(
        pattern, pattern, 0.7f, 0
    );
    
    // Apply attention to concept
    conceptForm.applyAttentionToConcept(conceptId);
    
    // Retrieve concept based on attentional focus
    size_t attendedConcept = conceptForm.getConceptBasedOnAttention();
    assert(attendedConcept == conceptId);
    
    std::cout << "    testConceptIntegrationWithAttention passed" << std::endl;
}

void testConceptSimilarityComputation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::ConceptFormation conceptForm;
    conceptForm.initialize(&brain);
    
    // Test similarity computation
    std::vector<float> pattern1{0.5f, 0.6f, 0.7f, 0.8f};
    std::vector<float> pattern2{0.6f, 0.7f, 0.8f, 0.9f};
    std::vector<float> pattern3{0.1f, 0.2f, 0.3f, 0.4f};
    
    float similarity12 = conceptForm.computeSimilarity(pattern1, pattern2);
    float similarity13 = conceptForm.computeSimilarity(pattern1, pattern3);
    
    // Similar patterns should have higher similarity
    assert(similarity12 > similarity13);
    
    // Test novelty detection
    bool isNovel = conceptForm.isNovel(pattern3, 0.8f);
    assert(isNovel);
    
    std::cout << "    testConceptSimilarityComputation passed" << std::endl;
}

void runAll() {
    testConceptCreation();
    testPrototypeEvolution();
    testConceptBasedOnAttention();
    testConceptIntegrationWithAttention();
    testConceptSimilarityComputation();
}

} // namespace test_concept_formation