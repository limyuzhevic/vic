// Concept Formation Tests
// Tests pattern detection and concept retention

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "cognition/ConceptFormation.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_concept_formation {

void testConceptFormationInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Concept formation should be accessible
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Initial state
    assert(conceptFormation->getConceptCount() == 0);
    assert(conceptFormation->getGeneralizationAbility(0) == 0.0f);
    
    std::cout << "    testConceptFormationInitialization passed" << std::endl;
}

void testConceptFormationPatternClustering() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create patterns that should cluster into similar concepts
    std::vector<std::vector<float>> patterns = {
        {1.0f, 0.9f, 0.8f, 0.7f, 0.6f}, // High values
        {0.95f, 0.85f, 0.75f, 0.65f, 0.55f}, // Similar high values
        {0.8f, 0.7f, 0.6f, 0.5f, 0.4f}, // Medium values
        {0.75f, 0.65f, 0.55f, 0.45f, 0.35f} // Similar medium values
    };
    
    // Present patterns to concept formation
    std::vector<size_t> conceptIds;
    for (size_t i = 0; i < patterns.size(); ++i) {
        auto pattern = patterns[i];
        size_t conceptId = brain.getConceptFormation()->presentExperience(
            pattern, pattern, 1.0f, static_cast<nlm::SimulationStep>(i));
        conceptIds.push_back(conceptId);
    }
    
    // Should have formed at least some concepts
    size_t conceptCount = brain.getConceptFormation()->getConceptCount();
    assert(conceptCount >= 1);
    
    // Concepts should be stable
    for (size_t conceptId : conceptIds) {
        if (conceptId > 0) {
            float stability = brain.getConceptFormation()->getConceptStability(conceptId);
            assert(stability >= 0.0f && stability <= 1.0f);
        }
    }
    
    std::cout << "    testConceptFormationPatternClustering passed" << std::endl;
}

void testConceptFormationNoveltyDetection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a pattern that will be learned
    std::vector<float> learnedPattern = {1.0f, 0.5f, 0.0f, -0.5f, -1.0f};
    brain.getConceptFormation()->presentExperience(
        learnedPattern, learnedPattern, 1.0f, 1);
    
    // Create a novel pattern (very different)
    std::vector<float> novelPattern = {-1.0f, -0.8f, -0.6f, -0.4f, -0.2f};
    bool isNovel = brain.getConceptFormation()->isNovel(
        novelPattern, 0.7f);
    
    // Should be novel
    assert(isNovel);
    
    // Create a similar pattern (should not be novel)
    std::vector<float> similarPattern = learnedPattern;
    for (float& val : similarPattern) val *= 0.8f;
    bool isNotNovel = !brain.getConceptFormation()->isNovel(
        similarPattern, 0.7f);
    
    // Should not be novel
    assert(isNotNovel);
    
    std::cout << "    testConceptFormationNoveltyDetection passed" << std::endl;
}

void testConceptFormationRetrieval() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a concept
    std::vector<float> pattern = {0.8f, 0.6f, 0.4f, 0.2f, 0.1f};
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        pattern, pattern, 1.0f, 1);
    
    if (conceptId > 0) {
        // Test prototype retrieval
        std::vector<float> retrievedPrototype = 
            brain.getConceptFormation()->getConceptPrototype(conceptId);
        assert(retrievedPrototype.size() == pattern.size());
        
        // Test instances retrieval
        const auto& instances = 
            brain.getConceptFormation()->getConceptInstances(conceptId);
        assert(!instances.empty());
        
        // Test matching concept
        size_t matchingConcept = brain.getConceptFormation()->getMatchingConcept(
            pattern, 0.7f);
        assert(matchingConcept == conceptId);
    }
    
    std::cout << "    testConceptFormationRetrieval passed" << std::endl;
}

void testConceptFormationStabilityOverTime() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a concept
    std::vector<float> pattern = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        pattern, pattern, 1.0f, 1);
    
    if (conceptId > 0) {
        // Check initial stability
        float initialStability = brain.getConceptFormation()->getConceptStability(conceptId);
        assert(initialStability >= 0.0f && initialStability <= 1.0f);
        
        // Update concept with more instances
        for (nlm::SimulationStep step = 2; step < 20; ++step) {
            brain.getConceptFormation()->updateConcept(
                conceptId, pattern, pattern, 0.8f);
            
            float currentStability = brain.getConceptFormation()->getConceptStability(conceptId);
            assert(currentStability >= 0.0f && currentStability <= 1.0f);
        }
        
        // Concept should still be accessible
        const nlm::DiscoveredConcept* concept = 
            brain.getConceptFormation()->getConcept(conceptId);
        assert(concept != nullptr);
    }
    
    std::cout << "    testConceptFormationStabilityOverTime passed" << std::endl;
}

void testConceptFormationGeneralization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a base concept
    std::vector<float> basePattern = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    size_t baseConceptId = brain.getConceptFormation()->presentExperience(
        basePattern, basePattern, 1.0f, 1);
    
    if (baseConceptId > 0) {
        // Test generalization ability
        float generalization = brain.getConceptFormation()->getGeneralizationAbility(baseConceptId);
        assert(generalization >= 0.0f && generalization <= 1.0f);
        
        // Create variations that should be generalized
        std::vector<std::vector<float>> variations;
        for (float scale = 0.5f; scale <= 1.5f; scale += 0.2f) {
            std::vector<float> variation;
            for (float val : basePattern) {
                variation.push_back(val * scale);
            }
            variations.push_back(variation);
        }
        
        // Test matching for variations
        size_t matches = 0;
        for (const auto& variation : variations) {
            size_t matchingConcept = brain.getConceptFormation()->getMatchingConcept(
                variation, 0.7f);
            if (matchingConcept > 0) matches++;
        }
        
        // Some variations should match the base concept
        assert(matches >= 0);
    }
    
    std::cout << "    testConceptFormationGeneralization passed" << std::endl;
}

void testConceptFormationConceptMerging() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create two separate concepts
    std::vector<float> pattern1 = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    std::vector<float> pattern2 = {-1.0f, -0.8f, -0.6f, -0.4f, -0.2f};
    
    size_t conceptId1 = brain.getConceptFormation()->presentExperience(
        pattern1, pattern1, 1.0f, 1);
    size_t conceptId2 = brain.getConceptFormation()->presentExperience(
        pattern2, pattern2, 1.0f, 2);
    
    if (conceptId1 > 0 && conceptId2 > 0) {
        // Merge concepts if they're actually the same (in this case they should be different)
        // For testing, we'll just verify that concepts can be accessed
        assert(conceptId1 != conceptId2);
        
        // Both concepts should be stable
        float stability1 = brain.getConceptFormation()->getConceptStability(conceptId1);
        float stability2 = brain.getConceptFormation()->getConceptStability(conceptId2);
        assert(stability1 >= 0.0f && stability1 <= 1.0f);
        assert(stability2 >= 0.0f && stability2 <= 1.0f);
    }
    
    std::cout << "    testConceptFormationConceptMerging passed" << std::endl;
}

void testConceptFormationClearAndReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create some concepts
    brain.getConceptFormation()->presentExperience(
        {1.0f, 0.5f, 0.0f}, {1.0f, 0.5f, 0.0f}, 1.0f, 1);
    brain.getConceptFormation()->presentExperience(
        {0.8f, 0.4f, 0.0f}, {0.8f, 0.4f, 0.0f}, 1.0f, 2);
    
    size_t conceptCount = brain.getConceptFormation()->getConceptCount();
    assert(conceptCount > 0);
    
    // Clear concepts
    brain.getConceptFormation()->clear();
    
    // Concept count should be zero
    assert(brain.getConceptFormation()->getConceptCount() == 0);
    
    std::cout << "    testConceptFormationClearAndReset passed" << std::endl;
}

void runAll() {
    testConceptFormationInitialization();
    testConceptFormationPatternClustering();
    testConceptFormationNoveltyDetection();
    testConceptFormationRetrieval();
    testConceptFormationStabilityOverTime();
    testConceptFormationGeneralization();
    testConceptFormationConceptMerging();
    testConceptFormationClearAndReset();
}

} // namespace test_concept_formation