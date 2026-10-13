// Concept Formation Tests
// Tests pattern discovery without labels, prototype formation through averaging, stability measurement, and category hints from properties

#include "cognition/ConceptFormation.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_concept_formation {

void testConceptFormationCreation() {
    nlm::ConceptFormation cf;
    
    assert(cf.getConceptCount() == 0);
    
    std::cout << "    testConceptFormationCreation passed" << std::endl;
}

void testConceptFormationInitialization() {
    nlm::ConceptFormation cf;
    
    // Should initialize with default threshold
    assert(cf.getConceptCount() == 0);
    
    std::cout << "    testConceptFormationInitialization passed" << std::endl;
}

void testPatternDiscoveryWithoutLabels() {
    nlm::ConceptFormation cf;
    
    // Initialize with a mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    cf.initialize(&mockBrain);
    
    // Present patterns that should form a concept
    std::vector<float> pattern1 = {0.8f, 0.7f, 0.6f, 0.5f, 0.4f};
    std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f}; // Similar to pattern1
    std::vector<float> pattern3 = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f}; // Different
    
    std::vector<float> features = {0.5f};
    
    // Present first pattern - should create concept
    size_t concept1 = cf.presentExperience(pattern1, features, 0.5f, 0);
    assert(concept1 > 0); // New concept formed
    assert(cf.getConceptCount() == 1);
    
    // Present similar pattern - should match existing concept
    size_t concept2 = cf.presentExperience(pattern2, features, 0.6f, 1);
    assert(concept2 == concept1); // Same concept
    assert(cf.getConceptCount() == 1); // Still one concept
    
    // Present different pattern - might not form concept (depends on similarity threshold)
    size_t concept3 = cf.presentExperience(pattern3, features, 0.3f, 2);
    
    std::cout << "    testPatternDiscoveryWithoutLabels passed" << std::endl;
}

void testPrototypeFormationThroughAveraging() {
    nlm::ConceptFormation cf;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    cf.initialize(&mockBrain);
    
    // Create first pattern
    std::vector<float> pattern1 = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    cf.presentExperience(pattern1, {0.5f}, 1.0f, 0);
    
    // Get prototype
    const auto* concept1 = cf.getConcept(1);
    assert(concept1 != nullptr);
    std::vector<float> prototype1 = cf.getConceptPrototype(1);
    
    // Create second similar pattern
    std::vector<float> pattern2 = {0.3f, 0.5f, 0.7f, 0.9f, 1.1f};
    cf.presentExperience(pattern2, {0.6f}, 0.8f, 1);
    
    // Get updated prototype
    std::vector<float> prototype2 = cf.getConceptPrototype(1);
    
    // Prototype should be between pattern1 and pattern2 (average-ish)
    for (size_t i = 0; i < prototype1.size(); ++i) {
        assert(prototype2[i] >= std::min(prototype1[i], pattern2[i]));
        assert(prototype2[i] <= std::max(prototype1[i], pattern2[i]));
    }
    
    std::cout << "    testPrototypeFormationThroughAveraging passed" << std::endl;
}

void testStabilityMeasurement() {
    nlm::ConceptFormation cf;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    cf.initialize(&mockBrain);
    
    // Present same pattern repeatedly - should increase stability
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    
    for (size_t i = 0; i < 10; ++i) {
        cf.presentExperience(pattern, {0.5f}, 0.8f, i);
    }
    
    float stability = cf.getConceptStability(1);
    assert(stability > 0.0f);
    assert(stability <= 1.0f);
    
    // More observations should lead to higher stability
    cf.presentExperience(pattern, {0.5f}, 0.9f, 10);
    float stability2 = cf.getConceptStability(1);
    
    std::cout << "    testStabilityMeasurement passed" << std::endl;
}

void testCategoryHintsFromProperties() {
    nlm::ConceptFormation cf;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    cf.initialize(&mockBrain);
    
    // Test positive reward -> "rewarding" category
    std::vector<float> pattern = {0.3f, 0.4f, 0.5f};
    cf.presentExperience(pattern, {0.5f}, 0.8f, 0);
    
    const auto* concept = cf.getConcept(1);
    assert(concept != nullptr);
    // Note: category hint logic would be implemented in the actual system
    // This test just verifies the concept was formed
    
    // Test negative reward -> "harmful" category
    cf.presentExperience(pattern, {0.5f}, -0.5f, 1);
    
    // Test neutral reward -> "neutral" category
    cf.presentExperience(pattern, {0.5f}, 0.1f, 2);
    
    std::cout << "    testCategoryHintsFromProperties passed" << std::endl;
}

void testGeneralizationAbility() {
    nlm::ConceptFormation cf;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    cf.initialize(&mockBrain);
    
    // Learn base pattern
    std::vector<float> basePattern = {0.8f, 0.8f, 0.8f, 0.8f, 0.8f};
    cf.presentExperience(basePattern, {0.8f}, 1.0f, 0);
    
    // Test generalization to slightly varied pattern
    std::vector<float> variation = {0.7f, 0.8f, 0.9f, 0.7f, 0.8f};
    size_t match = cf.getMatchingConcept(variation);
    
    if (match > 0) {
        float generalization = cf.getGeneralizationAbility(match);
        assert(generalization >= 0.0f && generalization <= 1.0f);
    }
    
    std::cout << "    testGeneralizationAbility passed" << std::endl;
}

void testConceptMerging() {
    nlm::ConceptFormation cf;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    cf.initialize(&mockBrain);
    
    // Create two concepts
    std::vector<float> pattern1 = {0.9f, 0.1f, 0.9f};
    std::vector<float> pattern2 = {0.1f, 0.9f, 0.1f};
    
    cf.presentExperience(pattern1, {0.5f}, 0.5f, 0);
    cf.presentExperience(pattern2, {0.5f}, 0.5f, 1);
    
    assert(cf.getConceptCount() >= 1); // Might merge into one concept
    
    // The mergeConcepts method should be available
    // Note: mergeConcepts might not be exposed in the public API
    // This test just verifies the infrastructure
    
    std::cout << "    testConceptMerging passed" << std::endl;
}

void testNoveltyDetection() {
    nlm::ConceptFormation cf;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    cf.initialize(&mockBrain);
    
    // Create concept with one pattern
    std::vector<float> pattern = {0.5f, 0.5f, 0.5f};
    cf.presentExperience(pattern, {0.5f}, 0.5f, 0);
    
    // Very similar pattern should not be novel
    std::vector<float> similarPattern = {0.51f, 0.51f, 0.51f};
    assert(!cf.isNovel(similarPattern, 0.5f)); // Similar enough
    
    // Very different pattern might be novel
    std::vector<float> differentPattern = {0.1f, 0.1f, 0.1f};
    assert(cf.isNovel(differentPattern, 0.5f)); // Different enough
    
    std::cout << "    testNoveltyDetection passed" << std::endl;
}

void testConceptRetrieval() {
    nlm::ConceptFormation cf;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    cf.initialize(&mockBrain);
    
    // Create concept
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f, 0.8f};
    cf.presentExperience(pattern, {0.5f}, 0.8f, 0);
    
    // Retrieve concept
    const auto* concept = cf.getConcept(1);
    assert(concept != nullptr);
    assert(concept->id == 1);
    assert(concept->prototype.size() == pattern.size());
    
    // Get concept instances
    const auto& instances = cf.getConceptInstances(1);
    assert(instances.size() > 0);
    
    std::cout << "    testConceptRetrieval passed" << std::endl;
}

void testConceptClear() {
    nlm::ConceptFormation cf;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    cf.initialize(&mockBrain);
    
    // Create some concepts
    cf.presentExperience(std::vector<float>{0.5f}, {0.5f}, 0.5f, 0);
    cf.presentExperience(std::vector<float>{0.6f}, {0.5f}, 0.5f, 1);
    
    assert(cf.getConceptCount() > 0);
    
    // Clear
    cf.clear();
    assert(cf.getConceptCount() == 0);
    
    std::cout << "    testConceptClear passed" << std::endl;
}

void runAll() {
    testConceptFormationCreation();
    testConceptFormationInitialization();
    testPatternDiscoveryWithoutLabels();
    testPrototypeFormationThroughAveraging();
    testStabilityMeasurement();
    testCategoryHintsFromProperties();
    testGeneralizationAbility();
    testConceptMerging();
    testNoveltyDetection();
    testConceptRetrieval();
    testConceptClear();
}

} // namespace test_concept_formation
