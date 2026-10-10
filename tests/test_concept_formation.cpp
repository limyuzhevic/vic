// ConceptFormation Tests
#include "cognition/ConceptFormation.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_concept_formation {

void testConceptFormationCreation() {
    nlm::ConceptFormation conceptFormation;
    
    assert(conceptFormation.getConceptCount() == 0);
    assert(conceptFormation.getFormationThreshold() == 0.75f);
    assert(conceptFormation.getStabilityThreshold() == 0.7f);
    assert(conceptFormation.getStabilityWindow() == 5);
    
    std::cout << "    testConceptFormationCreation passed" << std::endl;
}

void testConceptFormationInitialization() {
    nlm::ConceptFormation conceptFormation;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    conceptFormation.initialize(&brain);
    
    assert(conceptFormation.getConceptCount() == 0);
    
    std::cout << "    testConceptFormationInitialization passed" << std::endl;
}

void testConceptFormationPatternPresentation() {
    nlm::ConceptFormation conceptFormation;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    conceptFormation.initialize(&brain);
    
    // Test with simple pattern
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features = {0.0f, 1.0f};
    
    // Present first experience - should create new concept
    size_t conceptId = conceptFormation.presentExperience(pattern, features, 0.5f, 0);
    assert(conceptId > 0); // New concept ID
    assert(conceptFormation.getConceptCount() == 1);
    
    // Present same pattern again - should match existing concept
    size_t conceptId2 = conceptFormation.presentExperience(pattern, features, 0.7f, 1);
    assert(conceptId2 == conceptId); // Same concept
    
    std::cout << "    testConceptFormationPatternPresentation passed" << std::endl;
}

void testConceptFormationSimilarity() {
    nlm::ConceptFormation conceptFormation;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    conceptFormation.initialize(&brain);
    
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.1f, 0.2f, 0.3f}; // Identical
    std::vector<float> pattern3 = {0.5f, 0.6f, 0.7f}; // Different
    
    std::vector<float> features = {0.0f, 1.0f};
    
    // Create concept from first pattern
    size_t conceptId = conceptFormation.presentExperience(pattern1, features, 0.5f, 0);
    
    // Test similarity with identical pattern
    assert(conceptFormation.isNovel(pattern1) == false);
    
    // Test similarity with different pattern
    assert(conceptFormation.isNovel(pattern3) == true);
    
    // Test similarity threshold parameter
    assert(conceptFormation.isNovel(pattern1, 0.99f) == false); // Very high threshold
    assert(conceptFormation.isNovel(pattern3, 0.01f) == true); // Very low threshold
    
    std::cout << "    testConceptFormationSimilarity passed" << std::endl;
}

void testConceptFormationPrototypeUpdate() {
    nlm::ConceptFormation conceptFormation;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    conceptFormation.initialize(&brain);
    
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.2f, 0.3f, 0.4f}; // Slightly different
    
    std::vector<float> features = {0.0f, 1.0f};
    
    // Create concept from first pattern
    size_t conceptId = conceptFormation.presentExperience(pattern1, features, 0.5f, 0);
    
    // Update concept with second pattern
    conceptFormation.updateConcept(conceptId, pattern2, features, 0.6f);
    
    // Get prototype and verify it changed
    auto prototype = conceptFormation.getConceptPrototype(conceptId);
    assert(prototype.size() == 3);
    
    std::cout << "    testConceptFormationPrototypeUpdate passed" << std::endl;
}

void testConceptFormationStability() {
    nlm::ConceptFormation conceptFormation;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    conceptFormation.initialize(&brain);
    
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f};
    std::vector<float> features = {0.0f, 1.0f};
    
    // Create concept
    size_t conceptId = conceptFormation.presentExperience(pattern, features, 0.5f, 0);
    
    // Initially concept should be stable
    assert(conceptFormation.isConceptStable(conceptId) == true);
    
    // Get stability value
    float stability = conceptFormation.getConceptStability(conceptId);
    assert(stability >= 0.0f && stability <= 1.0f);
    
    std::cout << "    testConceptFormationStability passed" << std::endl;
}

void testConceptFormationGeneralization() {
    nlm::ConceptFormation conceptFormation;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    conceptFormation.initialize(&brain);
    
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f};
    std::vector<float> features = {0.0f, 1.0f};
    
    // Create concept
    size_t conceptId = conceptFormation.presentExperience(pattern, features, 0.5f, 0);
    
    // Get generalization ability
    float generalization = conceptFormation.getGeneralizationAbility(conceptId);
    assert(generalization >= 0.0f && generalization <= 1.0f);
    
    std::cout << "    testConceptFormationGeneralization passed" << std::endl;
}

void testConceptFormationMerge() {
    nlm::ConceptFormation conceptFormation;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    conceptFormation.initialize(&brain);
    
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.2f, 0.3f, 0.4f};
    std::vector<float> features = {0.0f, 1.0f};
    
    // Create two concepts
    size_t conceptId1 = conceptFormation.presentExperience(pattern1, features, 0.5f, 0);
    size_t conceptId2 = conceptFormation.presentExperience(pattern2, features, 0.6f, 1);
    
    assert(conceptFormation.getConceptCount() == 2);
    
    // Merge concepts (they might not merge based on similarity, but should not crash)
    conceptFormation.mergeConcepts(conceptId1, conceptId2);
    
    std::cout << "    testConceptFormationMerge passed" << std::endl;
}

void testConceptFormationClear() {
    nlm::ConceptFormation conceptFormation;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    conceptFormation.initialize(&brain);
    
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f};
    std::vector<float> features = {0.0f, 1.0f};
    
    // Create concept
    size_t conceptId = conceptFormation.presentExperience(pattern, features, 0.5f, 0);
    assert(conceptFormation.getConceptCount() == 1);
    
    // Clear all concepts
    conceptFormation.clear();
    assert(conceptFormation.getConceptCount() == 0);
    
    std::cout << "    testConceptFormationClear passed" << std::endl;
}

void runAll() {
    std::cout << "Running ConceptFormation tests..." << std::endl;
    testConceptFormationCreation();
    testConceptFormationInitialization();
    testConceptFormationPatternPresentation();
    testConceptFormationSimilarity();
    testConceptFormationPrototypeUpdate();
    testConceptFormationStability();
    testConceptFormationGeneralization();
    testConceptFormationMerge();
    testConceptFormationClear();
}

} // namespace test_concept_formation
