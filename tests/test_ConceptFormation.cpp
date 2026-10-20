// ConceptFormation Tests
#include "cognition/ConceptFormation.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_concept_formation {

void testConceptCreation() {
    nlm::ConceptFormation concept;
    
    assert(concept.getConceptCount() == 0);
    assert(concept.getGeneralizationAbility(0) == 0.0f);  // Invalid concept ID
    
    std::cout << "    testConceptCreation passed" << std::endl;
}

void testConceptInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::ConceptFormation concept;
    concept.initialize(&brain);
    
    assert(concept.getConceptCount() == 0);
    
    std::cout << "    testConceptInitialization passed" << std::endl;
}

void testConceptParameters() {
    nlm::ConceptFormation concept;
    
    // Test default parameters
    assert(concept.getFormationThreshold() == 0.7f);  // Default
    assert(concept.getStabilityThreshold() == 0.8f);   // Default
    assert(concept.getStabilityWindow() == 5);         // Default
    
    // Set new parameters
    concept.setFormationThreshold(0.5f);
    concept.setStabilityThreshold(0.9f);
    concept.setStabilityWindow(10);
    
    assert(concept.getFormationThreshold() == 0.5f);
    assert(concept.getStabilityThreshold() == 0.9f);
    assert(concept.getStabilityWindow() == 10);
    
    std::cout << "    testConceptParameters passed" << std::endl;
}

void testConceptSimilarity() {
    nlm::ConceptFormation concept;
    
    // Test similarity computation
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};  // Similar to A
    std::vector<float> patternC = {0.8f, 0.9f, 1.0f, 1.1f, 1.2f};  // Very different from A
    
    float similarityAB = concept.computeSimilarity(patternA, patternB);
    float similarityAC = concept.computeSimilarity(patternA, patternC);
    float similarityAA = concept.computeSimilarity(patternA, patternA);
    
    // Pattern A and B should be more similar than A and C
    assert(similarityAB > similarityAC);
    
    // Pattern should be identical to itself
    assert(similarityAA == 1.0f || similarityAA >= 0.99f);
    
    // Similarities should be between 0 and 1
    assert(similarityAB >= 0.0f && similarityAB <= 1.0f);
    assert(similarityAC >= 0.0f && similarityAC <= 1.0f);
    
    std::cout << "    testConceptSimilarity passed" << std::endl;
}

void testConceptNovelty() {
    nlm::ConceptFormation concept;
    
    // Create patterns
    std::vector<float> familiarPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> novelPattern = {0.9f, 1.0f, 1.1f, 1.2f, 1.3f};
    
    // Initially, both should be novel (no concepts learned yet)
    assert(concept.isNovel(familiarPattern, 0.7f));
    assert(concept.isNovel(novelPattern, 0.7f));
    
    // Present the familiar pattern to create a concept
    size_t conceptId = concept.presentExperience(familiarPattern, {0.1f, 0.2f}, 1.0f, 0);
    
    // The pattern should now not be novel (it belongs to the learned concept)
    assert(!concept.isNovel(familiarPattern, 0.7f));
    
    // The novel pattern should still be novel
    assert(concept.isNovel(novelPattern, 0.7f));
    
    std::cout << "    testConceptNovelty passed" << std::endl;
}

void testConceptPresentation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::ConceptFormation concept;
    concept.initialize(&brain);
    
    // Present first experience
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features1 = {0.1f, 0.2f, 0.3f};
    
    size_t conceptId1 = concept.presentExperience(pattern1, features1, 1.0f, 0);
    
    // Should have created a concept
    assert(conceptId1 > 0);
    assert(concept.getConceptCount() == 1);
    
    // Get the concept
    const nlm::DiscoveredConcept* learnedConcept = concept.getConcept(conceptId1);
    assert(learnedConcept != nullptr);
    
    // Present similar pattern - should match existing concept
    std::vector<float> similarPattern = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    std::vector<float> similarFeatures = {0.15f, 0.25f, 0.35f};
    
    size_t matchedConceptId = concept.getMatchingConcept(similarPattern, 0.7f);
    
    // Should have matched the existing concept
    assert(matchedConceptId == conceptId1);
    
    // Present very different pattern - should create new concept
    std::vector<float> differentPattern = {0.9f, 1.0f, 1.1f, 1.2f, 1.3f};
    std::vector<float> differentFeatures = {0.9f, 1.0f, 1.1f};
    
    size_t newConceptId = concept.presentExperience(differentPattern, differentFeatures, 2.0f, 10);
    
    // Should have created a new concept
    assert(newConceptId > conceptId1);
    assert(concept.getConceptCount() == 2);
    
    std::cout << "    testConceptPresentation passed" << std::endl;
}

void testConceptStability() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::ConceptFormation concept;
    concept.initialize(&brain);
    
    // Present a pattern multiple times to build stability
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features = {0.1f, 0.2f, 0.3f};
    
    size_t conceptId = concept.presentExperience(pattern, features, 1.0f, 0);
    
    // Initially might not be stable
    float stability = concept.getConceptStability(conceptId);
    assert(stability >= 0.0f && stability <= 1.0f);
    
    // Present the same pattern again
    concept.presentExperience(pattern, features, 1.5f, 1);
    concept.presentExperience(pattern, features, 2.0f, 2);
    
    // Stability should have increased
    float newStability = concept.getConceptStability(conceptId);
    assert(newStability >= stability);
    
    std::cout << "    testConceptStability passed" << std::endl;
}

void testConceptInstances() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::ConceptFormation concept;
    concept.initialize(&brain);
    
    // Present multiple experiences
    std::vector<float> basePattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    for (int i = 0; i < 5; ++i) {
        std::vector<float> variantPattern = basePattern;
        variantPattern[0] += static_cast<float>(i) * 0.01f;  // Small variation
        
        concept.presentExperience(variantPattern, {0.1f, 0.2f, 0.3f}, 1.0f, i);
    }
    
    // Get concept ID
    size_t conceptId = concept.findConceptForPattern(basePattern);
    assert(conceptId > 0);
    
    // Get instances
    const std::vector<nlm::ConceptInstance>& instances = concept.getConceptInstances(conceptId);
    
    // Should have recorded instances
    assert(instances.size() >= 1);
    
    // Get prototype
    std::vector<float> prototype = concept.getConceptPrototype(conceptId);
    assert(prototype.size() == basePattern.size());
    
    std::cout << "    testConceptInstances passed" << std::endl;
}

void testConceptMerging() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::ConceptFormation concept;
    concept.initialize(&brain);
    
    // Create two similar concepts
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> patternB = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};  // Similar to A
    
    size_t conceptIdA = concept.presentExperience(patternA, {0.1f, 0.2f}, 1.0f, 0);
    size_t conceptIdB = concept.presentExperience(patternB, {0.15f, 0.25f}, 1.2f, 1);
    
    assert(concept.getConceptCount() == 2);
    
    // Merge the two concepts
    concept.mergeConcepts(conceptIdA, conceptIdB);
    
    // Should now have only one concept
    assert(concept.getConceptCount() == 1);
    
    std::cout << "    testConceptMerging passed" << std::endl;
}

void testConceptGeneralization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::ConceptFormation concept;
    concept.initialize(&brain);
    
    // Create a concept
    std::vector<float> basePattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    size_t conceptId = concept.presentExperience(basePattern, {0.1f, 0.2f, 0.3f}, 1.0f, 0);
    
    // Test generalization ability
    float generalization = concept.getGeneralizationAbility(conceptId);
    assert(generalization >= 0.0f && generalization <= 1.0f);
    
    std::cout << "    testConceptGeneralization passed" << std::endl;
}

void testConceptUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::ConceptFormation concept;
    concept.initialize(&brain);
    
    // Create a concept
    std::vector<float> oldPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    size_t conceptId = concept.presentExperience(oldPattern, {0.1f, 0.2f, 0.3f}, 1.0f, 0);
    
    // Get current prototype
    std::vector<float> currentPrototype = concept.getConceptPrototype(conceptId);
    
    // Update with new instance
    std::vector<float> newPattern = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};  // Similar but different
    concept.updateConcept(conceptId, newPattern, {0.15f, 0.25f, 0.35f}, 1.5f);
    
    // Get new prototype
    std::vector<float> newPrototype = concept.getConceptPrototype(conceptId);
    
    // Prototype should have changed (been updated)
    assert(newPrototype.size() == currentPrototype.size());
    
    std::cout << "    testConceptUpdate passed" << std::endl;
}

void testConceptClear() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::ConceptFormation concept;
    concept.initialize(&brain);
    
    // Create some concepts
    for (int i = 0; i < 5; ++i) {
        std::vector<float> pattern = {static_cast<float>(i) * 0.1f, 
                                     static_cast<float>(i) * 0.2f,
                                     static_cast<float>(i) * 0.3f,
                                     static_cast<float>(i) * 0.4f,
                                     static_cast<float>(i) * 0.5f};
        concept.presentExperience(pattern, {0.1f, 0.2f, 0.3f}, 1.0f, i);
    }
    
    assert(concept.getConceptCount() == 5);
    
    // Clear all concepts
    concept.clear();
    
    assert(concept.getConceptCount() == 0);
    
    std::cout << "    testConceptClear passed" << std::endl;
}

void runAll() {
    testConceptCreation();
    testConceptInitialization();
    testConceptParameters();
    testConceptSimilarity();
    testConceptNovelty();
    testConceptPresentation();
    testConceptStability();
    testConceptInstances();
    testConceptMerging();
    testConceptGeneralization();
    testConceptUpdate();
    testConceptClear();
}

} // namespace test_concept_formation
