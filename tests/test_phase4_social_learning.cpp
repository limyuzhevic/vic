// test_phase4_social_learning.cpp
// Tests SocialLearning functionality

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <memory>

namespace nlm {

TEST_CASE("Phase 4 Social Learning Tests", "[social_learning]") {
    
    SECTION("Test SocialLearning initialization") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* socialLearning = planner.getSocialLearning();
        REQUIRE(socialLearning != nullptr);
        
        // Test basic social learning properties
        REQUIRE(socialLearning->hasSocialKnowledge() == false);  // No observations yet
        REQUIRE(socialLearning->getObservationCount() == 0);
    }
    
    SECTION("Test SocialLearning observation recording") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* socialLearning = planner.getSocialLearning();
        REQUIRE(socialLearning != nullptr);
        
        // Record observations of other agents
        std::vector<float> observerState(30, 0.5f);
        std::vector<float> resultingState = observerState;
        ActionType observedAction = ActionType::MoveForward;
        
        socialLearning->observeAgentAction(observedAction, observerState, resultingState);
        
        // Should have recorded an observation
        REQUIRE(socialLearning->hasSocialKnowledge() == true);
        REQUIRE(socialLearning->getObservationCount() == 1);
        
        // Should be able to imitate the observed action
        REQUIRE(socialLearning->canImitate(observedAction) == true);
    }
    
    SECTION("Test SocialLearning imitation learning") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* socialLearning = planner.getSocialLearning();
        REQUIRE(socialLearning != nullptr);
        
        // Observe an action
        std::vector<float> observerState(30, 0.5f);
        std::vector<float> resultingState = observerState;
        ActionType observedAction = ActionType::Interact;
        
        socialLearning->observeAgentAction(observedAction, observerState, resultingState);
        
        // Learn to imitate
        ActionType imitation = socialLearning->getImitationAction(observerState);
        
        // Should imitate the observed action
        REQUIRE(imitation == observedAction);
    }
    
    SECTION("Test SocialLearning multiple observations") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* socialLearning = planner.getSocialLearning();
        REQUIRE(socialLearning != nullptr);
        
        // Observe multiple actions
        std::vector<float> state1(30, 0.5f);
        std::vector<float> state2(30, 0.8f);
        
        ActionType action1 = ActionType::MoveForward;
        ActionType action2 = ActionType::TurnLeft;
        
        socialLearning->observeAgentAction(action1, state1, state1);
        socialLearning->observeAgentAction(action2, state2, state2);
        
        // Should be able to imitate both actions
        REQUIRE(socialLearning->canImitate(action1) == true);
        REQUIRE(socialLearning->canImitate(action2) == true);
        REQUIRE(socialLearning->getObservationCount() == 2);
        
        // Should be able to select correct action based on state
        ActionType imitation1 = socialLearning->getImitationAction(state1);
        ActionType imitation2 = socialLearning->getImitationAction(state2);
        
        // Should match observed actions (though could choose either)
        REQUIRE(imitation1 == action1 || imitation1 == action2);
        REQUIRE(imitation2 == action1 || imitation2 == action2);
    }
    
    SECTION("Test SocialLearning communication signal learning") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* socialLearning = planner.getSocialLearning();
        REQUIRE(socialLearning != nullptr);
        
        // Learn communication signal
        std::vector<float> signalPattern(20, 0.5f);
        float signalReward = 0.8f;
        
        socialLearning->learnCommunicationSignal(signalPattern, signalReward);
        
        // Should be able to detect the signal
        bool signalDetected = socialLearning->detectSignal(signalPattern);
        REQUIRE(signalDetected == true);
        
        // Get signal meaning
        float meaning = socialLearning->getSignalMeaning();
        REQUIRE(meaning == signalReward);
        
        // Get signal pattern
        std::vector<float> retrievedPattern = socialLearning->getSignalPattern();
        REQUIRE(retrievedPattern.size() == signalPattern.size());
    }
    
    SECTION("Test SocialLearning social knowledge update") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* socialLearning = planner.getSocialLearning();
        REQUIRE(socialLearning != nullptr);
        
        // Observe an action
        std::vector<float> observerState(30, 0.5f);
        std::vector<float> resultingState = observerState;
        ActionType observedAction = ActionType::MoveForward;
        
        socialLearning->observeAgentAction(observedAction, observerState, resultingState);
        
        // Update social knowledge based on interaction reward
        float interactionReward = 0.7f;
        socialLearning->updateSocialKnowledge(interactionReward);
        
        // Should have updated knowledge
        REQUIRE(socialLearning->hasSocialKnowledge() == true);
        
        // Can still imitate
        REQUIRE(socialLearning->canImitate(observedAction) == true);
    }
}

} // namespace nlm
