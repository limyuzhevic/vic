// test_phase4_integration.cpp
// Tests the integration of Phase 4 cognitive systems
// 
// Tests:
// - Neural planner integration with AgentBrain
// - Concept formation integration with AgentBrain
// - Attention integration with AgentBrain
// - Self-model integration with AgentBrain
// - Social learning integration with AgentBrain

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <memory>

namespace nlm {

TEST_CASE("Phase 4 Integration Tests", "[integration]") {
    
    SECTION("Test Brain cognitive system initialization") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(cfg);
        REQUIRE(brain->initialize());
        
        // Test that cognitive systems are properly initialized
        REQUIRE(brain->getPlanner() != nullptr);
        REQUIRE(brain->getConceptFormation() != nullptr);
        REQUIRE(brain->getAttention() != nullptr);
        
        // Initialize cognitive systems
        brain->getPlanner()->initialize(brain.get());
        brain->getConceptFormation()->initialize(brain.get());
        brain->getAttention()->initialize(brain.get());
        
        NLM_LOG_INFO("Phase 4 Integration Tests: All cognitive systems initialized");
    }
    
    SECTION("Test NeuralPlanner integration") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        brain->getPlanner()->initialize(brain.get());
        
        // Test basic planner functionality
        std::vector<float> testState(50, 0.5f);
        ActionType action = brain->getPlanner()->planAction(testState, 0.5f);
        
        // Should return a valid action
        REQUIRE(action != ActionType::Wait || brain->getPlanner()->getPlanningConfidence() > 0.0f);
        
        // Test action quality updates
        brain->getPlanner()->setActionQuality(ActionType::MoveForward, 0.7f);
        brain->getPlanner()->setActionQuality(ActionType::Interact, 0.8f);
        
        float quality = brain->getPlanner()->getPlanningConfidence();
        REQUIRE(quality >= 0.0f);
    }
    
    SECTION("Test ConceptFormation integration") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        brain->getConceptFormation()->initialize(brain.get());
        
        // Test concept formation
        std::vector<float> testPattern(30, 0.8f);
        std::vector<float> features = {0.7f};
        
        size_t conceptId = brain->getConceptFormation()->presentExperience(testPattern, features, 0.8f, 1);
        
        // Should create a concept
        REQUIRE(conceptId > 0);
        
        // Test concept retrieval
        size_t matchingConcept = brain->getConceptFormation()->getMatchingConcept(testPattern, 0.7f);
        REQUIRE(matchingConcept == conceptId);
        
        // Test concept stability
        float stability = brain->getConceptFormation()->getConceptStability(conceptId);
        REQUIRE(stability >= 0.0f && stability <= 1.0f);
    }
    
    SECTION("Test AttentionalSelection integration") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        brain->getAttention()->initialize(brain.get());
        
        // Test attention initialization
        brain->getAttention()->setInhibitionStrength(0.5f);
        brain->getAttention()->setExcitationStrength(1.5f);
        
        REQUIRE(brain->getAttention()->getInhibitionStrength() == 0.5f);
        REQUIRE(brain->getAttention()->getExcitationStrength() == 1.5f);
        
        // Test attention processing
        std::vector<NeuronId> competitors;
        for (int i = 0; i < 5; ++i) {
            competitors.push_back(NeuronId(50000 + i));
        }
        
        brain->getAttention()->applyBottomUpSalience(competitors[0], 1.0f);
        auto winners = brain->getAttention()->processCompetition(competitors);
        
        // Should select at least one winner
        REQUIRE(!winners.empty());
    }
    
    SECTION("Test SelfModel integration via NeuralPlanner") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        brain->getPlanner()->initialize(brain.get());
        
        // Test self-model via planner
        auto* selfModel = brain->getPlanner()->getSelfModel();
        REQUIRE(selfModel != nullptr);
        
        // Test basic self-model functionality
        std::vector<float> beforeState(30, 0.5f);
        std::vector<float> afterState = beforeState;
        
        ActionType action = ActionType::MoveForward;
        selfModel->recordSelfAction(action, beforeState, afterState);
        
        // Test prediction
        auto prediction = selfModel->predictActionConsequence(action, beforeState);
        REQUIRE(!prediction.empty());
        
        // Test confidence
        float confidence = selfModel->getSelfModelConfidence(action);
        REQUIRE(confidence >= 0.0f);
    }
    
    SECTION("Test SocialLearning integration via NeuralPlanner") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        brain->getPlanner()->initialize(brain.get());
        
        // Test social learning via planner
        auto* socialLearning = brain->getPlanner()->getSocialLearning();
        REQUIRE(socialLearning != nullptr);
        
        // Test social learning observation
        std::vector<float> observerState(30, 0.5f);
        std::vector<float> resultingState = observerState;
        ActionType observedAction = ActionType::Interact;
        
        socialLearning->observeAgentAction(observedAction, observerState, resultingState);
        
        // Should be able to imitate observed action
        REQUIRE(socialLearning->canImitate(observedAction));
        
        // Test imitation
        ActionType imitation = socialLearning->getImitationAction(observerState);
        REQUIRE(imitation == observedAction);
    }
}

} // namespace nlm
