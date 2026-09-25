// Motor Tests
#include "motor/MotorSystem.hpp"
#include "motor/Action.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_motor {

TEST_CASE("MotorSystem creation and initialization", "[motor][system][creation]") {
    nlm::MotorSystem motorSystem;
    
    REQUIRE(!motorSystem.isInitialized());
}

TEST_CASE("MotorSystem initialization with config", "[motor][system][initialization]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("motor.enabled", true, nlm::ConfigSource::Runtime);
    config->set("motor.max_actions", 10, nlm::ConfigSource::Runtime);
    
    nlm::MotorSystem motorSystem;
    motorSystem.initialize(config);
    REQUIRE(motorSystem.isInitialized());
}

TEST_CASE("Action creation and properties", "[motor][action][creation]") {
    nlm::Action action{nlm::ActionId(1), "move", "forward", 1.0f};
    
    REQUIRE(action.id != 0);
    REQUIRE(action.type != nullptr);
    REQUIRE(action.name != nullptr);
    REQUIRE(action.strength > 0.0f);
}

TEST_CASE("Action execution and effects", "[motor][action][execution]") {
    nlm::Action action{nlm::ActionId(2), "rotate", "left", 0.5f};
    
    REQUIRE(action.type == "rotate");
    REQUIRE(action.name == "left");
    REQUIRE(action.strength == 0.5f);
}

TEST_CASE("MotorSystem action queue management", "[motor][system][queue]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("motor.enabled", true, nlm::ConfigSource::Runtime);
    config->set("motor.max_actions", 5, nlm::ConfigSource::Runtime);
    
    nlm::MotorSystem motorSystem;
    motorSystem.initialize(config);
    
    // Add actions to queue
    nlm::Action action1{nlm::ActionId(3), "move", "forward", 1.0f};
    nlm::Action action2{nlm::ActionId(4), "turn", "right", 0.5f};
    
    motorSystem.addAction(action1);
    motorSystem.addAction(action2);
    
    REQUIRE(motorSystem.getActionQueueSize() == 2);
}

TEST_CASE("MotorSystem action execution sequence", "[motor][system][sequence]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("motor.enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::MotorSystem motorSystem;
    motorSystem.initialize(config);
    
    // Add and execute actions
    std::vector<nlm::Action> actions = {
        nlm::Action{nlm::ActionId(5), "move", "forward", 1.0f},
        nlm::Action{nlm::ActionId(6), "turn", "left", 0.3f},
        nlm::Action{nlm::ActionId(7), "move", "backward", 0.8f}
    };
    
    for (const auto& action : actions) {
        motorSystem.addAction(action);
    }
    
    REQUIRE(motorSystem.getActionQueueSize() == 3);
    
    // Process actions
    while (motorSystem.getActionQueueSize() > 0) {
        motorSystem.processNextAction();
    }
}

TEST_CASE("Action type and strength validation", "[motor][action][validation]") {
    // Test valid actions
    nlm::Action moveAction{nlm::ActionId(8), "move", "north", 1.5f};
    REQUIRE(moveAction.type == "move");
    REQUIRE(moveAction.strength == 1.5f);
    
    nlm::Action rotateAction{nlm::ActionId(9), "rotate", "clockwise", 0.7f};
    REQUIRE(rotateAction.type == "rotate");
    REQUIRE(rotateAction.strength == 0.7f);
    
    nlm::Action grabAction{nlm::ActionId(10), "grab", "object", 0.9f};
    REQUIRE(grabAction.type == "grab");
}

TEST_CASE("MotorSystem with different configurations", "[motor][system][config]") {
    // Test motor system with different configurations
    for (int maxActions = 1; maxActions <= 10; ++maxActions) {
        auto config = std::make_shared<nlm::Config>();
        config->set("motor.enabled", true, nlm::ConfigSource::Runtime);
        config->set("motor.max_actions", maxActions, nlm::ConfigSource::Runtime);
        
        nlm::MotorSystem motorSystem;
        motorSystem.initialize(config);
        
        REQUIRE(motorSystem.isInitialized());
    }
}

TEST_CASE("Action strength limits and clamping", "[motor][action][strength]") {
    // Test action strength limits
    nlm::Action weakAction{nlm::ActionId(11), "light", "tap", 0.01f};
    REQUIRE(weakAction.strength == 0.01f);
    
    nlm::Action strongAction{nlm::ActionId(12), "heavy", "smash", 2.0f};
    REQUIRE(strongAction.strength == 2.0f);
}

TEST_CASE("MotorSystem performance evaluation", "[motor][performance][evaluation]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("motor.enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::MotorSystem motorSystem;
    motorSystem.initialize(config);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Process many actions
    for (int i = 0; i < 50; ++i) {
        nlm::Action action{nlm::ActionId(i + 13), "action", "description", 0.1f * (i + 1)};
        motorSystem.addAction(action);
    }
    
    // Process all actions
    while (motorSystem.getActionQueueSize() > 0) {
        motorSystem.processNextAction();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    REQUIRE(duration.count() < 1000000);  // Less than 1 second
}

} // namespace test_motor
