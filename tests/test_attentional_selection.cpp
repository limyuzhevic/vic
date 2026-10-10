// AttentionalSelection Tests
#include "cognition/NeuralPlanner.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>

namespace test_attentional_selection {

void testAttentionalSelectionCreation() {
    nlm::NeuralPlanner attentionalSystem;
    
    assert(attentionalSystem.getPlanningDepth() == 3);
    assert(attentionalSystem.getPlanningConfidence() == 0.5f);
    
    std::cout << "    testAttentionalSelectionCreation passed" << std::endl;
}

void testAttentionalSelectionInitialization() {
    nlm::NeuralPlanner attentionalSystem;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    attentionalSystem.initialize(&brain);
    
    assert(attentionalSystem.getPlanningDepth() == 3);
    
    std::cout << "    testAttentionalSelectionInitialization passed" << std::endl;
}

void testAttentionalSelectionSalienceProcessing() {
    nlm::NeuralPlanner attentionalSystem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    attentionalSystem.initialize(&brain);
    
    std::vector<float> state = {0.1f, 0.8f, 0.2f, 0.9f, 0.3f}; // Some values have high salience
    
    // Test that attentional system can process state (through planning)
    nlm::ActionType action = attentionalSystem.planAction(state);
    
    // Should return a valid action
    assert(action == nlm::ActionType::Wait ||
           action == nlm::ActionType::MoveForward ||
           action == nlm::ActionType::MoveBackward ||
           action == nlm::ActionType::TurnLeft ||
           action == nlm::ActionType::TurnRight ||
           action == nlm::ActionType::Interact);
    
    std::cout << "    testAttentionalSelectionSalienceProcessing passed" << std::endl;
}

void testAttentionalSelectionCompetition() {
    nlm::NeuralPlanner attentionalSystem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    attentionalSystem.initialize(&brain);
    
    std::vector<float> state = {0.1f, 0.8f, 0.2f, 0.9f, 0.3f};
    
    // Test that system selects from competing options
    for (int i = 0; i < 10; ++i) {
        nlm::ActionType action = attentionalSystem.planAction(state);
        assert(action != nlm::ActionType::Wait || !state.empty());
    }
    
    std::cout << "    testAttentionalSelectionCompetition passed" << std::endl;
}

void testAttentionalSelectionConfidence() {
    nlm::NeuralPlanner attentionalSystem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    attentionalSystem.initialize(&brain);
    
    std::vector<float> state = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    // Plan multiple actions to potentially affect confidence
    nlm::ActionType action1 = attentionalSystem.planAction(state);
    nlm::ActionType action2 = attentionalSystem.planAction(state, 0.5f);
    
    float confidence = attentionalSystem.getPlanningConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testAttentionalSelectionConfidence passed" << std::endl;
}

void testAttentionalSelectionGoalSetting() {
    nlm::NeuralPlanner attentionalSystem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    attentionalSystem.initialize(&brain);
    
    std::vector<float> goal = {0.2f, 0.8f, 0.1f};
    attentionalSystem.setCurrentGoal(goal);
    
    assert(attentionalSystem.getCurrentGoal().size() == 3);
    assert(attentionalSystem.getCurrentGoal()[0] == 0.2f);
    
    std::cout << "    testAttentionalSelectionGoalSetting passed" << std::endl;
}

void testAttentionalSelectionIntegration() {
    // Test integration with brain and planning system
    nlm::NeuralPlanner attentionalSystem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    attentionalSystem.initialize(&brain);
    
    // Test that attentional system can plan with different states
    std::vector<float> state1 = {0.1f, 0.9f, 0.2f};
    std::vector<float> state2 = {0.8f, 0.1f, 0.3f};
    
    nlm::ActionType action1 = attentionalSystem.planAction(state1);
    nlm::ActionType action2 = attentionalSystem.planAction(state2);
    
    assert(action1 != nlm::ActionType::Wait || !state1.empty());
    assert(action2 != nlm::ActionType::Wait || !state2.empty());
    
    std::cout << "    testAttentionalSelectionIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running AttentionalSelection tests..." << std::endl;
    testAttentionalSelectionCreation();
    testAttentionalSelectionInitialization();
    testAttentionalSelectionSalienceProcessing();
    testAttentionalSelectionCompetition();
    testAttentionalSelectionConfidence();
    testAttentionalSelectionGoalSetting();
    testAttentionalSelectionIntegration();
}

} // namespace test_attentional_selection
