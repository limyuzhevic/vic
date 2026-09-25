// test_phase4_concept_formation.cpp
// Tests ConceptFormation functionality

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <memory>

namespace nlm {

TEST_CASE("Phase 4 Concept Formation Tests", "[concept_formation]") {
    
    SECTION("Test ConceptFormation initialization") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        ConceptFormation concepts;
        concepts.initialize(brain.get());
        
        // Test default parameters
        REQUIRE(concepts.getConceptCount() == 0);
        REQUIRE(concepts.isNovel(std::vector<float>(30, 0.5f)));  // No concepts yet
        
        // Test parameter setting
        concepts.setFormationThreshold(0.5f);
        concepts.setStabilityWindow(5);
    }
    
    SECTION("Test ConceptFormation pattern presentation") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        ConceptFormation concepts;
        concepts.initialize(brain.get());
        
        // Present a pattern
        std::vector<float> pattern(30, 0.8f);
        std::vector<float> features = {0.7f};
        
        size_t conceptId = concepts.presentExperience(pattern, features, 0.8f, 1);
        
        // Should create a concept
        REQUIRE(conceptId > 0);
        
        REQUIRE(concepts.getConceptCount() == 1);
        
        // Test concept stability
        float stability = concepts.getConceptStability(conceptId);
        REQUIRE(stability >= 0.0f && stability <= 1.0f);
    }
    
    SECTION("Test ConceptFormation similarity computation") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        ConceptFormation concepts;
        concepts.initialize(brain.get());
        
        // Create identical patterns
        std::vector<float> patternA(30, 0.5f);
        std::vector<float> patternB(30, 0.5f);
        
        float similarity = concepts.computeSimilarity(patternA, patternB);
        REQUIRE(similarity > 0.9f);  // Should be very similar
        
        // Create very different patterns
        std::vector<float> patternC(30, 0.0f);
        std::vector<float> patternD(30, 1.0f);
        
        float dissimilarity = concepts.computeSimilarity(patternC, patternD);
        REQUIRE(dissimilarity < 0.1f);  // Should be very dissimilar
    }
    
    SECTION("Test ConceptFormation concept merging") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        ConceptFormation concepts;
        concepts.initialize(brain.get());
        
        // Create two similar concepts
        std::vector<float> pattern1(30, 0.5f);
        std::vector<float> features = {0.7f};
        
        size_t concept1 = concepts.presentExperience(pattern1, features, 0.8f, 1);
        
        std::vector<float> pattern2 = pattern1;  // Very similar
        for (auto& v : pattern2) v += 0.01f;  // Small variation
        
        size_t concept2 = concepts.presentExperience(pattern2, features, 0.7f, 2);
        
        REQUIRE(concept1 > 0 && concept2 > 0);
        REQUIRE(concept1 != concept2);
        
        // Merge similar concepts
        concepts.mergeConcepts(concept1, concept2);
        
        // Should have one less concept now
        REQUIRE(concepts.getConceptCount() == 1);
    }
    
    SECTION("Test ConceptFormation generalization ability") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        ConceptFormation concepts;
        concepts.initialize(brain.get());
        
        // Create a concept
        std::vector<float> basePattern(30, 0.5f);
        std::vector<float> features = {0.7f};
        
        size_t conceptId = concepts.presentExperience(basePattern, features, 0.8f, 1);
        
        // Test generalization with small variation
        std::vector<float> variant = basePattern;
        for (auto& v : variant) {
            v += (brain->getRandomGenerator()->uniformReal(0.0f, 0.1f)) - 0.05f;
            v = std::clamp(v, 0.0f, 1.0f);
        }
        
        size_t matchingConcept = concepts.getMatchingConcept(variant, 0.8f);
        
        // Should match the original concept
        REQUIRE(matchingConcept == conceptId);
        
        float generalization = concepts.getGeneralizationAbility(conceptId);
        REQUIRE(generalization >= 0.0f && generalization <= 1.0f);
    }
}

} // namespace nlm
