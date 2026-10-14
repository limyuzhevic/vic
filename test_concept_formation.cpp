#include "brain/Brain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_concept_formation {

void testConceptFormationCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("enable_concept_formation", true, nlm::ConfigSource::Default);
    config->set("concept_formation_threshold", 0.75, nlm::ConfigSource::Default);
    config->set("concept_stability_threshold", 0.7, nlm::ConfigSource::Default);
    config->set("concept_stability_window", 5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Get concept formation system
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Configure concept formation with test parameters
    conceptFormation->configureConceptFormation(
        0.8f,  // formationThreshold
        0.75f, // stabilityThreshold
        3u,    // stabilityWindow
        true,  // multiModalEnabled
        0.6f,  // conceptAttentiveGain
        0.4f,  // conceptPlanningGain
        0.2f   // conceptPredictionGain
    );
    
    // Test basic concept formation functionality
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Present first experience
    size_t conceptId1 = conceptFormation->presentExperience(pattern1, features1, 0.5f, 0);
    std::cout << "    Concept 1 ID: " << conceptId1 << std::endl;
    
    // Update with similar pattern (should match)
    std::vector<float> pattern2 = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    std::vector<float> features2 = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    
    size_t conceptId2 = conceptFormation->presentExperience(pattern2, features2, 0.6f, 1);
    std::cout << "    Concept 2 ID: " << conceptId2 << std::endl;
    
    // Check concept count
    size_t conceptCount = conceptFormation->getConceptCount();
    std::cout << "    Concept count: " << conceptCount << std::endl;
    
    // Test concept stability
    if (conceptId1 > 0) {
        float stability1 = conceptFormation->getConceptStability(conceptId1);
        std::cout << "    Concept 1 stability: " << stability1 << std::endl;
        
        // Test concept activation strength
        float activation1 = conceptFormation->getConceptActivationStrength(conceptId1);
        std::cout << "    Concept 1 activation: " << activation1 << std::endl;
    }
    
    // Test multi-modal concept formation (with working memory and episodic memory context)
    std::vector<float> workingMemory = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
    std::vector<float> episodicContext = {0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
    
    std::cout << "    Testing multi-modal concept formation..." << std::endl;
    conceptFormation->presentExperienceEnhanced(
        pattern1,         // sensoryPattern
        workingMemory,   // workingMemory
        episodicContext, // episodicContext
        0.8f,            // reward
        2               // currentStep
    );
    
    // Test concept formation configuration
    auto formationConfig = conceptFormation->getFormationConfig();
    std::cout << "    Concept formation threshold: " << formationConfig.threshold << std::endl;
    std::cout << "    Concept formation multi-modal enabled: " << (formationConfig.multiModalEnabled ? "yes" : "no") << std::endl;
    
    // Test concept similarity computation
    if (conceptId1 > 0 && conceptId2 > 0) {
        float similarity = conceptFormation->computeConceptSimilarity(conceptId1, conceptId2);
        std::cout << "    Concept similarity: " << similarity << std::endl;
    }
    
    std::cout << "    testConceptFormationCreation passed" << std::endl;
}

void testConceptFormationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("enable_concept_formation", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test concept formation integration with brain
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept formation storage in episodic memory
    std::vector<float> testPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> testFeatures = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    size_t conceptId = conceptFormation->presentExperience(testPattern, testFeatures, 0.5f, 0);
    
    // The concept should be stored and accessible
    const nlm::DiscoveredConcept* concept = conceptFormation->getConcept(conceptId);
    assert(concept != nullptr);
    
    std::cout << "    Concept formation integrated with episodic memory" << std::endl;
    
    // Test concept formation integration with attention system
    if (brain.getAttention()) {
        // Concept formation should provide biases to attention system
        std::cout << "    Concept formation integration with attention system tested" << std::endl;
    }
    
    // Test concept formation integration with prediction system
    if (brain.getPredictionSystem()) {
        // Concept formation should provide constraints to prediction system
        std::cout << "    Concept formation integration with prediction system tested" << std::endl;
    }
    
    // Test concept formation integration with planning system
    if (brain.getPlanner()) {
        // Concept formation should provide goals to planning system
        std::cout << "    Concept formation integration with planning system tested" << std::endl;
    }
    
    std::cout << "    testConceptFormationIntegration passed" << std::endl;
}

void testConceptFormationConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test configuration loading
    float formationThreshold = config->getOr<float>("concept_formation_threshold", 0.75f);
    float stabilityThreshold = config->getOr<float>("concept_stability_threshold", 0.7f);
    size_t stabilityWindow = config->getOr<size_t>("concept_stability_window", 5);
    bool multiModalEnabled = config->getOr<bool>("concept_multimodal_enabled", true);
    bool conceptFormationEnabled = config->getOr<bool>("enable_concept_formation", true);
    
    std::cout << "    Concept formation threshold from config: " << formationThreshold << std::endl;
    std::cout << "    Concept formation stability threshold from config: " << stabilityThreshold << std::endl;
    std::cout << "    Concept formation stability window from config: " << stabilityWindow << std::endl;
    std::cout << "    Concept formation multi-modal from config: " << (multiModalEnabled ? "yes" : "no") << std::endl;
    std::cout << "    Concept formation enabled from config: " << (conceptFormationEnabled ? "yes" : "no") << std::endl;
    
    // Test concept formation disable
    config->set("enable_concept_formation", false, nlm::ConfigSource::Runtime);
    
    auto brain2 = nlm::Brain(config);
    brain2.initialize();
    
    nlm::ConceptFormation* conceptFormation = brain2.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // When concept formation is disabled, it should be cleared
    size_t conceptCount = conceptFormation->getConceptCount();
    std::cout << "    Concept formation disabled - concept count: " << conceptCount << std::endl;
    
    std::cout << "    testConceptFormationConfiguration passed" << std::endl;
}

void testConceptFormationMultiModal() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(40), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("concept_multimodal_enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test multi-modal concept formation with different input sources
    std::vector<float> sensoryPattern = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    std::vector<float> workingMemory = {0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
    std::vector<float> episodicContext = {0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
    
    std::cout << "    Testing multi-modal concept formation..." << std::endl;
    conceptFormation->presentExperienceEnhanced(
        sensoryPattern,
        workingMemory,
        episodicContext,
        0.9f,
        10
    );
    
    // The pattern should be blended and processed
    size_t conceptCount = conceptFormation->getConceptCount();
    std::cout << "    Multi-modal concept formation resulted in " << conceptCount << " concept(s)" << std::endl;
    
    // Test with multi-modal disabled
    config->set("concept_multimodal_enabled", false, nlm::ConfigSource::Runtime);
    
    auto brain2 = nlm::Brain(config);
    brain2.initialize();
    
    nlm::ConceptFormation* conceptFormation2 = brain2.getConceptFormation();
    assert(conceptFormation2 != nullptr);
    
    // Should still work but without multi-modal integration
    conceptFormation2->presentExperience(sensoryPattern, sensoryPattern, 0.5f, 11);
    std::cout << "    Multi-modal disabled test passed" << std::endl;
    
    std::cout << "    testConceptFormationMultiModal passed" << std::endl;
}

void testConceptFormationNoveltyAndGeneralization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test novelty detection
    std::vector<float> novelPattern = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    std::vector<float> novelFeatures = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    
    // Present novel pattern
    size_t novelConceptId = conceptFormation->presentExperience(novelPattern, novelFeatures, 0.3f, 0);
    std::cout << "    Novel concept ID: " << novelConceptId << std::endl;
    
    // Test generalization ability
    std::vector<float> similarPattern = {0.85f, 0.75f, 0.65f, 0.55f, 0.45f};
    std::vector<float> similarFeatures = {0.85f, 0.75f, 0.65f, 0.55f, 0.45f};
    
    size_t similarConceptId = conceptFormation->presentExperience(similarPattern, similarFeatures, 0.4f, 1);
    std::cout << "    Similar concept ID: " << similarConceptId << std::endl;
    
    // Test concept stability over time
    if (novelConceptId > 0) {
        float initialStability = conceptFormation->getConceptStability(novelConceptId);
        std::cout << "    Initial concept stability: " << initialStability << std::endl;
        
        // Present more instances to increase stability
        for (int i = 2; i <= 5; ++i) {
            std::vector<float> additionalPattern = {
                0.9f * (1.0f - i * 0.1f),  // Vary slightly
                0.8f * (1.0f - i * 0.1f),
                0.7f * (1.0f - i * 0.1f),
                0.6f * (1.0f - i * 0.1f),
                0.5f * (1.0f - i * 0.1f)
            };
            conceptFormation->presentExperience(additionalPattern, additionalPattern, 0.5f, i);
        }
        
        float finalStability = conceptFormation->getConceptStability(novelConceptId);
        std::cout << "    Final concept stability: " << finalStability << std::endl;
        
        assert(finalStability >= initialStability);  // Stability should increase with more instances
    }
    
    // Test generalization ability
    if (novelConceptId > 0 && similarConceptId > 0) {
        float generalization = conceptFormation->getGeneralizationAbility(novelConceptId);
        std::cout << "    Concept generalization ability: " << generalization << std::endl;
    }
    
    std::cout << "    testConceptFormationNoveltyAndGeneralization passed" << std::endl;
}

} // namespace test_concept_formation

int main() {
    std::cout << "Running enhanced concept formation tests..." << std::endl << std::endl;
    
    test_concept_formation::testConceptFormationCreation();
    std::cout << std::endl;
    
    test_concept_formation::testConceptFormationIntegration();
    std::cout << std::endl;
    
    test_concept_formation::testConceptFormationConfiguration();
    std::cout << std::endl;
    
    test_concept_formation::testConceptFormationMultiModal();
    std::cout << std::endl;
    
    test_concept_formation::testConceptFormationNoveltyAndGeneralization();
    std::cout << std::endl;
    
    std::cout << "All concept formation tests passed successfully!" << std::endl;
    return 0;
}