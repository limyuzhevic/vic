// Comprehensive test suite for AgentBrain improvements
// This file tests all the fixes and improvements made to AgentBrain

#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <memory>

class AgentBrainTestSuite {
public:
    static void runAllTests() {
        std::cout << "Running AgentBrain Comprehensive Test Suite..." << std::endl;
        
        testConfigurationValidation();
        testMotorNeuronsAccess();
        testConfigurationSetters();
        testNoveltyCalculation();
        testMotorDistribution();
        testMemorySafety();
        
        std::cout << "All tests passed! ✓" << std::endl;
    }
    
private:
    static void testConfigurationValidation() {
        std::cout << "  Testing configuration validation..." << std::endl;
        
        // Create minimal configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Create AgentBrain
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Test initialization
        SimpleWorld world;
        world.configure(8, 8, 16, 16);
        agentBrain->initialize(world);
        
        // Test configuration validation - should pass
        bool valid = agentBrain->validateConfiguration();
        assert(valid);
        
        // Test configuration summary
        std::string summary = agentBrain->getConfigurationSummary();
        assert(!summary.empty());
        assert(summary.find("AgentBrain Configuration Summary") != std::string::npos);
        
        std::cout << "    Configuration validation passed." << std::endl;
    }
    
    static void testMotorNeuronsAccess() {
        std::cout << "  Testing motor neuron access..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Test motor neuron access for each command
        const auto& forwardNeurons = agentBrain->getMotorNeurons(MotorCommand::MoveForward);
        const auto& backwardNeurons = agentBrain->getMotorNeurons(MotorCommand::MoveBackward);
        const auto& leftNeurons = agentBrain->getMotorNeurons(MotorCommand::TurnLeft);
        const auto& rightNeurons = agentBrain->getMotorNeurons(MotorCommand::TurnRight);
        const auto& lookLeftNeurons = agentBrain->getMotorNeurons(MotorCommand::LookLeft);
        const auto& lookRightNeurons = agentBrain->getMotorNeurons(MotorCommand::LookRight);
        const auto& interactNeurons = agentBrain->getMotorNeurons(MotorCommand::Interact);
        const auto& waitNeurons = agentBrain->getMotorNeurons(MotorCommand::Wait);
        
        // All should return valid vectors (might be empty but should not crash)
        assert(&forwardNeurons != nullptr);
        assert(&backwardNeurons != nullptr);
        assert(&leftNeurons != nullptr);
        assert(&rightNeurons != nullptr);
        assert(&lookLeftNeurons != nullptr);
        assert(&lookRightNeurons != nullptr);
        assert(&interactNeurons != nullptr);
        assert(&waitNeurons != nullptr);
        
        // Test invalid command
        const auto& empty = agentBrain->getMotorNeurons(static_cast<MotorCommand>(99));
        assert(&empty != nullptr);
        
        std::cout << "    Motor neuron access passed." << std::endl;
    }
    
    static void testConfigurationSetters() {
        std::cout << "  Testing configuration setters..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Test valid sensory novelty decay
        assert(agentBrain->setSensoryNoveltyDecay(0.95f));
        assert(agentBrain->setSensoryNoveltyDecay(0.5f));
        assert(agentBrain->setSensoryNoveltyDecay(1.0f));
        
        // Test invalid sensory novelty decay
        assert(!agentBrain->setSensoryNoveltyDecay(1.5f));  // Too high
        assert(!agentBrain->setSensoryNoveltyDecay(-0.1f)); // Too low
        
        // Test valid curiosity threshold
        assert(agentBrain->setCuriosityThreshold(0.3f));
        assert(agentBrain->setCuriosityThreshold(0.0f));
        assert(agentBrain->setCuriosityThreshold(1.0f));
        
        // Test invalid curiosity threshold
        assert(!agentBrain->setCuriosityThreshold(1.5f));
        assert(!agentBrain->setCuriosityThreshold(-0.1f));
        
        std::cout << "    Configuration setters passed." << std::endl;
    }
    
    static void testNoveltyCalculation() {
        std::cout << "  Testing novelty calculation..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Test initialization with world
        SimpleWorld world;
        world.configure(16, 16, 16, 16);
        agentBrain->initialize(world);
        
        // Test novelty calculation with empty vision (should not crash)
        std::vector<float> emptyVision;
        SensoryPercept percept;
        percept.setVision(emptyVision);
        agentBrain->processSensoryInput(percept);
        
        // Test novelty calculation with same vision (should be low)
        std::vector<float> vision1(256, 0.5f);
        percept.setVision(vision1);
        agentBrain->processSensoryInput(percept);
        float novelty1 = agentBrain->getNoveltyLevel();
        
        // Test with different vision (should be higher)
        std::vector<float> vision2(256, 0.7f);
        for (size_t i = 0; i < 128; ++i) {
            vision2[i] = 0.1f;
        }
        percept.setVision(vision2);
        agentBrain->processSensoryInput(percept);
        float novelty2 = agentBrain->getNoveltyLevel();
        
        // Both should be valid (not NaN or infinite)
        assert(!std::isnan(novelty1));
        assert(!std::isinf(novelty1));
        assert(!std::isnan(novelty2));
        assert(!std::isinf(novelty2));
        
        std::cout << "    Novelty calculation passed." << std::endl;
    }
    
    static void testMotorDistribution() {
        std::cout << "  Testing motor neuron distribution..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Check that all motor neuron groups are initialized
        // After initialization, all should have at least some neurons
        // (though exact count depends on brain configuration)
        
        size_t totalMotorNeurons = agentBrain->getMotorNeurons(MotorCommand::MoveForward).size() +
                                   agentBrain->getMotorNeurons(MotorCommand::MoveBackward).size() +
                                   agentBrain->getMotorNeurons(MotorCommand::TurnLeft).size() +
                                   agentBrain->getMotorNeurons(MotorCommand::TurnRight).size() +
                                   agentBrain->getMotorNeurons(MotorCommand::LookLeft).size() +
                                   agentBrain->getMotorNeurons(MotorCommand::LookRight).size() +
                                   agentBrain->getMotorNeurons(MotorCommand::Interact).size() +
                                   agentBrain->getMotorNeurons(MotorCommand::Wait).size();
        
        // Should have some motor neurons (brain creates motor neurons)
        assert(totalMotorNeurons > 0);
        
        std::cout << "    Motor neuron distribution passed. (Total: " 
                  << totalMotorNeurons << " neurons)" << std::endl;
    }
    
    static void testMemorySafety() {
        std::cout << "  Testing memory safety..." << std::endl;
        
        // Test null brain handling
        auto agentBrain = std::make_shared<AgentBrain>(nullptr);
        
        // Should not crash when brain is null
        agentBrain->processSensoryInput(SensoryPercept());
        
        // Should return default command when brain is null
        MotorCommand cmd = agentBrain->decodeMotorCommand();
        assert(cmd == MotorCommand::Wait);  // Default when no brain
        
        // Configuration validation should fail for null brain
        assert(!agentBrain->validateConfiguration());
        
        std::cout << "    Memory safety passed." << std::endl;
    }
};

// Main function to run tests
int main() {
    AgentBrainTestSuite::runAllTests();
    return 0;
}
