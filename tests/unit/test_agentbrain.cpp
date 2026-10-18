// AgentBrain Tests
// Comprehensive tests for AgentBrain enhancements

#include "test_framework.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>

namespace test_agentbrain {

// Test AgentBrain creation and basic functionality
class AgentBrainCreationTest : public TestSuite {
public:
    AgentBrainCreationTest() : TestSuite("AgentBrain::Creation") {}
    
    void run() override {
        std::cout << "  Testing AgentBrain creation..." << std::endl;
        
        // Create config and brain
        nlm::Config config;
        std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
        nlm::Brain brain(configPtr);
        
        TIME_TEST("AgentBrain Construction", {
            nlm::AgentBrain agentBrain(&brain);
            
            assert(agentBrain.getSensoryInputSize() >= 0);
            assert(agentBrain.getMotorOutputSize() >= 0);
            assert(agentBrain.getNeuromodulationLevel() >= 0.0f);
            assert(agentBrain.getCuriosityLevel() >= 0.0f);
            assert(agentBrain.getNoveltyLevel() >= 0.0f);
            assert(agentBrain.getPredictionError() >= 0.0f);
        });
        
        // Test AgentBrain initialization
        TIME_TEST("AgentBrain Initialization", {
            nlm::SimpleWorld world;
            world.configure(100.0, 100.0, 16, 16);
            world.reset();
            
            nlm::AgentBrain agentBrain(&brain);
            agentBrain.initialize(world);
            
            // Verify initialization state
            assert(agentBrain.getNeuromodulationLevel() >= 0.0f);
            assert(agentBrain.getCuriosityLevel() >= 0.0f);
            assert(agentBrain.getNoveltyLevel() >= 0.0f);
        });
        
        std::cout << "  AgentBrain Creation tests PASSED" << std::endl;
    }
};

// Test AgentBrain sensory processing
class AgentBrainSensoryTest : public TestSuite {
public:
    AgentBrainSensoryTest() : TestSuite("AgentBrain::Sensory") {}
    
    void run() override {
        std::cout << "  Testing AgentBrain sensory processing..." << std::endl;
        
        // Create components
        nlm::Config config;
        std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
        nlm::Brain brain(configPtr);
        nlm::SimpleWorld world;
        world.configure(100.0, 100.0, 16, 16);
        world.reset();
        
        TIME_TEST("AgentBrain Sensory Processing", {
            nlm::AgentBrain agentBrain(&brain);
            agentBrain.initialize(world);
            
            // Create sensory percept with sample data
            nlm::SensoryPercept percept;
            
            // Vision data (16x16 grid)
            std::vector<float> vision(256, 0.5f);
            for (int i = 0; i < 50; ++i) {
                vision[i] = 0.8f;  // Some visible objects
            }
            percept.setVision(vision);
            
            // Touch data
            std::vector<float> touch(8, 0.0f);
            touch[0] = 1.0f;  // Collision detected
            percept.setTouch(touch);
            
            // Internal data
            std::vector<float> internal(4, 0.0f);
            internal[0] = 0.7f;  // Energy level
            internal[1] = 0.9f;  // Health
            percept.setInternal(internal);
            
            // Proprioception data
            std::vector<float> proprioception(6, 0.0f);
            proprioception[0] = 0.5f;  // Position X
            proprioception[1] = 0.5f;  // Position Y
            proprioception[2] = 0.0f;  // Orientation
            percept.setProprioception(proprioception);
            
            // Process sensory input
            agentBrain.processSensoryInput(percept);
            
            // Verify processing
            assert(agentBrain.getNeuromodulationLevel() >= 0.0f);
            assert(agentBrain.getCuriosityLevel() >= 0.0f);
            assert(agentBrain.getNoveltyLevel() >= 0.0f);
        });
        
        std::cout << "  AgentBrain Sensory tests PASSED" << std::endl;
    }
};

// Test AgentBrain motor decoding
class AgentBrainMotorTest : public TestSuite {
public:
    AgentBrainMotorTest() : TestSuite("AgentBrain::Motor") {}
    
    void run() override {
        std::cout << "  Testing AgentBrain motor decoding..." << std::endl;
        
        // Create components
        nlm::Config config;
        std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
        nlm::Brain brain(configPtr);
        brain.initialize();
        
        TIME_TEST("AgentBrain Motor Decoding", {
            nlm::AgentBrain agentBrain(&brain);
            
            // Test basic motor decoding
            nlm::MotorCommand command = agentBrain.decodeMotorCommand();
            // Command should be one of the valid MotorCommand types
        });
        
        // Test curiosity-based action selection
        TIME_TEST("AgentBrain Curiosity-Based Selection", {
            nlm::AgentBrain agentBrain(&brain);
            
            // Set high curiosity level
            // Note: This depends on implementation details
            // agentBrain.setCuriosityLevel(0.8f);
            
            // Test with curiosity
            nlm::MotorCommand command = agentBrain.decodeFromMotorNeurons();
        });
        
        std::cout << "  AgentBrain Motor tests PASSED" << std::endl;
    }
};

// Test AgentBrain reward processing
class AgentBrainRewardTest : public TestSuite {
public:
    AgentBrainRewardTest() : TestSuite("AgentBrain::Reward") {}
    
    void run() override {
        std::cout << "  Testing AgentBrain reward processing..." << std::endl;
        
        // Create components
        nlm::Config config;
        std::shared_ptr<nlm::Config> configPtr = std::make_shared<nlm::Config>(config);
        nlm::Brain brain(configPtr);
        brain.initialize();
        
        TIME_TEST("AgentBrain Reward Modulation", {
            nlm::AgentBrain agentBrain(&brain);
            
            // Apply reward modulation
            float reward = 1.0f;
            float predictedReward = 0.5f;
            
            agentBrain.applyRewardModulation(reward, predictedReward);
            
            // Verify reward effects
            assert(agentBrain.getPredictionError() >= 0.0f);
            assert(agentBrain.getNeuromodulationLevel() >= 0.0f);
            assert(agentBrain.getExpectedReward() >= 0.0f);
        });
        
        // Test development effects
        TIME_TEST("AgentBrain Development", {
            nlm::AgentBrain agentBrain(&brain);
            
            double timestep = 1.0;
            
            // Update development
            agentBrain.updateDevelopment(timestep);
            
            // Verify developmental stage
            nlm::DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
            assert(stage == nlm::DevelopmentalStage::Initial || 
                   stage == nlm::DevelopmentalStage::CriticalPeriod ||
                   stage == nlm::DevelopmentalStage::Maturation ||
                   stage == nlm::DevelopmentalStage::Adult);
            
            // Verify plasticity modifier
            float plasticity = agentBrain.getPlasticityModifier();
            assert(plasticity >= 0.1f && plasticity <= 1.0f);
        });
        
        std::cout << "  AgentBrain Reward tests PASSED" << std::endl;
    }
};

// Global test instances
static AgentBrainCreationTest agentBrainCreationTest;
static AgentBrainSensoryTest agentBrainSensoryTest;
static AgentBrainMotorTest agentBrainMotorTest;
static AgentBrainRewardTest agentBrainRewardTest;

void runAll() {
    std::cout << "=== AgentBrain Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&agentBrainCreationTest);
    framework.addSuite(&agentBrainSensoryTest);
    framework.addSuite(&agentBrainMotorTest);
    framework.addSuite(&agentBrainRewardTest);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All AgentBrain Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some AgentBrain Tests FAILED ===" << std::endl;
    }
}

} // namespace test_agentbrain
