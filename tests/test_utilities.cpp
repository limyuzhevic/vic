// NLM Utilities Test
// Testing the new utility functions added to NLM

#include <iostream>
#include <cassert>
#include "core/utilities/Utilities.hpp"

namespace test_utilities {

void testMotorCommandToString() {
    std::cout << "Testing motorCommandToString..." << std::endl;
    
    // Test basic command names
    assert(motorCommandToString(MotorCommand::MoveForward) == "MoveForward");
    assert(motorCommandToString(MotorCommand::MoveBackward) == "MoveBackward");
    assert(motorCommandToString(MotorCommand::TurnLeft) == "TurnLeft");
    assert(motorCommandToString(MotorCommand::TurnRight) == "TurnRight");
    assert(motorCommandToString(MotorCommand::LookLeft) == "LookLeft");
    assert(motorCommandToString(MotorCommand::LookRight) == "LookRight");
    assert(motorCommandToString(MotorCommand::Interact) == "Interact");
    assert(motorCommandToString(MotorCommand::Wait) == "Wait");
    assert(motorCommandToString(MotorCommand::MoveForward) == "MoveForward");
    
    std::cout << "  PASSED" << std::endl;
}

void testDevelopmentalStageToString() {
    std::cout << "Testing developmentalStageToString..." << std::endl;
    
    // Test developmental stage names
    assert(developmentalStageToString(DevelopmentalStage::Initial) == "Initial");
    assert(developmentalStageToString(DevelopmentalStage::CriticalPeriod) == "CriticalPeriod");
    assert(developmentalStageToString(DevelopmentalStage::Maturation) == "Maturation");
    assert(developmentalStageToString(DevelopmentalStage::Adult) == "Adult");
    assert(developmentalStageToString(DevelopmentalStage::Aging) == "Aging");
    
    std::cout << "  PASSED" << std::endl;
}

void testCreateDefaultConfig() {
    std::cout << "Testing createDefaultConfig..." << std::endl;
    
    // Test with default experiment type
    auto config = createDefaultConfig("general");
    assert(config != nullptr);
    assert(config->getOr<int64_t>("random_seed", 0) == 42);
    assert(config->getOr<double>("simulation_timestep", 0.0) == 0.001);
    assert(config->getOr<int64_t>("neuron_count", 0) == 500);
    assert(config->getOr<int64_t>("region_count", 0) == 2);
    assert(config->getOr<float>("connection_probability", 1.0f) == 0.1f);
    
    // Test with phase3 experiment type
    auto config3 = createDefaultConfig("phase3");
    assert(config3 != nullptr);
    assert(config3->getOr<int64_t>("environment_width", 0) == 20);
    assert(config3->getOr<int64_t>("environment_height", 0) == 20);
    assert(config3->getOr<int64_t>("vision_width", 0) == 16);
    assert(config3->getOr<int64_t>("vision_height", 0) == 16);
    
    // Test with phase6 experiment type
    auto config6 = createDefaultConfig("phase6");
    assert(config6 != nullptr);
    assert(config6->getOr<int64_t>("environment_width", 0) == 16);
    assert(config6->getOr<int64_t>("environment_height", 0) == 16);
    assert(config6->getOr<int64_t>("vision_width", 0) == 16);
    assert(config6->getOr<int64_t>("vision_height", 0) == 16);
    assert(config6->getOr<int64_t>("max_steps", 0) == 2000);
    
    std::cout << "  PASSED" << std::endl;
}

void testValidateConfig() {
    std::cout << "Testing validateConfig..." << std::endl;
    
    // Test with null config
    assert(!validateConfig(nullptr));
    
    // Test with valid config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("simulation_timestep", 0.001);
    assert(validateConfig(config));
    
    // Test with invalid config (missing neuron_count)
    auto config2 = std::make_shared<Config>();
    assert(!validateConfig(config2));
    
    // Test with invalid timestep
    auto config3 = std::make_shared<Config>();
    config3->set("neuron_count", 100);
    config3->set("simulation_timestep", 0.0);  // Invalid timestep
    assert(!validateConfig(config3));
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== NLM Utilities Tests ===" << std::endl;
    
    try {
        testMotorCommandToString();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    try {
        testDevelopmentalStageToString();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    try {
        testCreateDefaultConfig();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    try {
        testValidateConfig();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << std::endl << "=== All Utilities Tests PASSED ===" << std::endl;
}

} // namespace test_utilities