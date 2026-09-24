// AgentBrain Test Suite
// Comprehensive testing of AgentBrain functionality including:
// - Basic initialization and configuration
// - Sensory processing
// - Motor command decoding
// - Reward modulation
// - Development system
// - Curiosity and novelty detection
// - Error handling and validation
// - Performance monitoring

#include "AgentBrain.hpp"
#include "Brain.hpp"
#include "Config.hpp"
#include "SimpleWorld.hpp"
#include "SensoryPercept.hpp"
#include "Action.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

// Test helper functions
namespace {
    
    // Helper to create a minimal valid brain for testing
    std::shared_ptr<Brain> createTestBrain() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize test brain");
        }
        return brain;
    }
    
    // Helper to create a test world
    std::shared_ptr<SimpleWorld> createTestWorld() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);  // width, height, visionWidth, visionHeight
        world->reset();
        return world;
    }
    
    // Helper to create a test percept
    SensoryPercept createTestPercept() {
        SensoryPercept percept;
        
        // Create vision data (8x8 = 64 values)
        std::vector<float> vision(64, 0.5f);
        for (size_t i = 0; i < vision.size(); ++i) {
            vision[i] = static_cast<float>(i) / vision.size();
        }
        percept.setVision(vision);
        
        // Create touch data (8 values)
        std::vector<float> touch(8, 0.0f);
        touch[0] = 0.8f;  // Object detected
        percept.setTouch(touch);
        
        // Create internal signals (4 values)
        std::vector<float> internal(4, 0.0f);
        internal[0] = 0.3f;  // Hunger level
        internal[1] = 0.7f;  // Energy level
        percept.setInternal(internal);
        
        // Create proprioception (6 values)
        std::vector<float> proprioception(6, 0.0f);
        proprioception[0] = 0.0f;  // X position
        proprioception[1] = 0.0f;  // Y position
        proprioception[2] = 0.0f;  // Orientation
        percept.setProprioception(proprioception);
        
        return percept;
    }
    
    // Helper to verify motor command validity
    bool isValidMotorCommand(MotorCommand cmd) {
        return cmd == MotorCommand::MoveForward ||
               cmd == MotorCommand::MoveBackward ||
               cmd == MotorCommand::TurnLeft ||
               cmd == MotorCommand::TurnRight ||
               cmd == MotorCommand::Interact ||
               cmd == MotorCommand::Wait ||
               cmd == MotorCommand::LookLeft ||
               cmd == MotorCommand::LookRight;
    }
}

// Test class for AgentBrain
class AgentBrainTest {
public:
    static void runAllTests() {
        std::cout << "=== Running AgentBrain Test Suite ===" << std::endl;
        
        try {
            testConstructorAndInitialization();
            testConfiguration();
            testSensoryProcessing();
            testMotorDecoding();
            testRewardModulation();
            testDevelopmentSystem();
            testCuriosityAndNovelty();
            testErrorHandling();
            testPerformanceMonitoring();
            testValidation();
            
            std::cout << std::endl;
            std::cout << "=== All AgentBrain Tests Passed ===" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << std::endl;
            std::cout << "=== Test Failed ===" << std::endl;
            std::cout << "Error: " << e.what() << std::endl;
            throw;
        }
    }
    
private:
    static void testConstructorAndInitialization() {
        std::cout << "Test 1: Constructor and Initialization..." << std::endl;
        
        // Test default constructor
        auto brain = createTestBrain();
        AgentBrain agentBrain(brain);
        
        assert(agentBrain.getBrain() == brain.get());
        assert(agentBrain.getNeuromodulationLevel() == 0.0f);
        assert(agentBrain.getCuriosityLevel() == 0.0f);
        assert(agentBrain.getNoveltyLevel() == 0.0f);
        assert(agentBrain.getPredictionError() == 0.0f);
        
        // Test initialization with default config
        auto world = createTestWorld();
        agentBrain.initialize(*world);
        
        assert(agentBrain.getSensoryInputSize() > 0);
        assert(agentBrain.getMotorOutputSize() > 0);
        
        // Test initialization with custom config
        AgentBrain::Config config;
        config.visionScaleFactor = 2.0f;
        config.curiosityThreshold = 0.5f;
        
        AgentBrain agentBrain2(brain, config);
        assert(agentBrain2.getVisionScaleFactor() == 2.0f);
        assert(agentBrain2.getCuriosityThreshold() == 0.5f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    static void testConfiguration() {
        std::cout << "Test 2: Configuration..." << std::endl;
        
        auto brain = createTestBrain();
        auto world = createTestWorld();
        
        // Test default configuration
        AgentBrain::Config defaultConfig = AgentBrain::getDefaultConfig();
        assert(defaultConfig.visionScaleFactor == 5.0f);
        assert(defaultConfig.curiosityThreshold == 0.3f);
        assert(defaultConfig.explorationChance == 0.3f);
        assert(defaultConfig.rewardClampingMin == -1.0f);
        assert(defaultConfig.rewardClampingMax == 1.0f);
        
        // Test configuration setters
        AgentBrain agentBrain(brain, defaultConfig);
        agentBrain.initialize(*world);
        
        agentBrain.setVisionScaleFactor(3.0f);
        assert(agentBrain.getVisionScaleFactor() == 3.0f);
        
        agentBrain.setCuriosityThreshold(0.7f);
        assert(agentBrain.getCuriosityThreshold() == 0.7f);
        
        agentBrain.setExplorationChance(0.5f);
        assert(agentBrain.getExplorationChance() == 0.5f);
        
        agentBrain.setRewardClamping(-2.0f, 2.0f);
        assert(agentBrain.getRewardClampingMin() == -2.0f);
        assert(agentBrain.getRewardClampingMax() == 2.0f);
        
        // Test callback setup
        agentBrain.setCuriosityCalculator(
            [](float novelty, float predictionError, float multiplier, float weight) {
                return novelty * multiplier + weight * predictionError;
            }
        );
        
        agentBrain.setActionSelector(
            [](MotorCommand defaultCmd, float curiosityLevel, float noveltyLevel) {
                if (curiosityLevel > 0.8f && noveltyLevel > 0.5f) {
                    return MotorCommand::Interact;
                }
                return defaultCmd;
            }
        );
        
        std::cout << "  PASSED" << std::endl;
    }
    
    static void testSensoryProcessing() {
        std::cout << "Test 3: Sensory Processing..." << std::endl;
        
        auto brain = createTestBrain();
        auto world = createTestWorld();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(*world);
        
        // Create test percept
        SensoryPercept percept = createTestPercept();
        
        // Process sensory input
        agentBrain.processSensoryInput(percept);
        
        // Verify processing occurred (should not crash)
        assert(agentBrain.getSensoryNeuronCount() > 0);
        
        // Test with empty percept (should not crash)
        SensoryPercept emptyPercept;
        agentBrain.processSensoryInput(emptyPercept);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    static void testMotorDecoding() {
        std::cout << "Test 4: Motor Decoding..." << endl;
        
        auto brain = createTestBrain();
        auto world = createTestWorld();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(*world);
        
        // Decode motor command (should not crash)
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        
        // Verify command is valid
        assert(isValidMotorCommand(cmd));
        
        // Test with high curiosity (should use curiosity-based exploration)
        agentBrain.enableCuriosity(true);
        
        // Modify internal state to trigger curiosity
        agentBrain.getCuriosityLevel();  // Just access the level
        
        std::cout << "  PASSED" << std::endl;
    }
    
    static void testRewardModulation() {
        std::cout << "Test 5: Reward Modulation..." << endl;
        
        auto brain = createTestBrain();
        auto world = createTestWorld();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(*world);
        
        // Apply reward modulation
        agentBrain.applyRewardModulation(0.8f, 0.5f);  // reward=0.8, predicted=0.5
        
        // Verify prediction error was calculated
        float predictionError = agentBrain.getPredictionError();
        assert(predictionError == 0.3f);  // 0.8 - 0.5 = 0.3
        
        // Verify expected reward was updated
        assert(agentBrain.getNeuromodulationLevel() == 0.3f);  // Same as prediction error
        
        // Apply negative reward
        agentBrain.applyRewardModulation(-0.5f, 0.2f);  // reward=-0.5, predicted=0.2
        
        float predictionError2 = agentBrain.getPredictionError();
        assert(predictionError2 == -0.7f);  // -0.5 - 0.2 = -0.7
        
        // Test reward clamping
        agentBrain.setRewardClamping(-0.5f, 0.5f);
        agentBrain.applyRewardModulation(2.0f, 1.0f);  // Should be clamped
        
        float clampedDopamine = agentBrain.getNeuromodulationLevel();
        assert(clampedDopamine <= 0.5f);  // Should be clamped to max
        
        std::cout << "  PASSED" << std::endl;
    }
    
    static void testDevelopmentSystem() {
        std::cout << "Test 6: Development System..." << endl;
        
        auto brain = createTestBrain();
        auto world = createTestWorld();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(*world);
        
        // Test development update
        agentBrain.updateDevelopment(10.0);  // 10 seconds
        
        // Verify developmental stage was updated
        DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
        assert(stage == DevelopmentalStage::Initial || stage == DevelopmentalStage::CriticalPeriod);
        
        // Test multiple development updates
        agentBrain.updateDevelopment(200.0);  // 200 seconds - should move to Maturation
        DevelopmentalStage stage2 = agentBrain.getDevelopmentalStage();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    static void testCuriosityAndNovelty() {
        std::cout << "Test 7: Curiosity and Novelty..." << endl;
        
        auto brain = createTestBrain();
        auto world = createTestWorld();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(*world);
        
        // Create test percept with changing vision
        SensoryPercept percept1 = createTestPercept();
        SensoryPercept percept2 = createTestPercept();
        
        // Modify second percept to create novelty
        auto vision2 = percept2.getVision();
        if (!vision2.empty()) {
            vision2[0] = 1.0f;  // Different from first (0.0f)
            percept2.setVision(vision2);
        }
        
        // Process both percepts
        agentBrain.processSensoryInput(percept1);
        float novelty1 = agentBrain.getNoveltyLevel();
        
        agentBrain.processSensoryInput(percept2);
        float novelty2 = agentBrain.getNoveltyLevel();
        
        // Verify novelty was calculated
        assert(!std::isnan(novelty1));
        assert(!std::isnan(novelty2));
        
        // Test curiosity calculation
        float curiosity = agentBrain.getCuriosityLevel();
        assert(curiosity >= 0.0f && curiosity <= 1.0f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    static void testErrorHandling() {
        std::cout << "Test 8: Error Handling..." << endl;
        
        // Test with null brain
        AgentBrain nullAgentBrain(nullptr);
        
        // These should not crash
        nullAgentBrain.getSensoryInputSize();
        nullAgentBrain.getMotorOutputSize();
        nullAgentBrain.decodeMotorCommand();
        nullAgentBrain.getNeuromodulationLevel();
        nullAgentBrain.getCuriosityLevel();
        nullAgentBrain.getNoveltyLevel();
        nullAgentBrain.getPredictionError();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    static void testPerformanceMonitoring() {
        std::cout << "Test 9: Performance Monitoring..." << endl;
        
        auto brain = createTestBrain();
        auto world = createTestWorld();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(*world);
        
        // Get initial step count
        size_t initialSteps = agentBrain.getStepCount();
        
        // Perform some operations
        SensoryPercept percept = createTestPercept();
        agentBrain.processSensoryInput(percept);
        agentBrain.decodeMotorCommand();
        agentBrain.applyRewardModulation(0.5f, 0.3f);
        agentBrain.updateDevelopment(1.0);
        
        // Step count should be updated
        size_t finalSteps = agentBrain.getStepCount();
        assert(finalSteps >= initialSteps);
        
        // Test status string
        std::string status = agentBrain.getStatusString();
        assert(!status.empty());
        
        std::cout << "  PASSED" << std::endl;
    }
    
    static void testValidation() {
        std::cout << "Test 10: Validation..." << endl;
        
        auto brain = createTestBrain();
        auto world = createTestWorld();
        
        AgentBrain agentBrain(brain);
        agentBrain.initialize(*world);
        
        // Test configuration validation
        bool configValid = agentBrain.validateConfiguration();
        assert(configValid);
        
        // Test neural connectivity validation
        bool connectivityValid = agentBrain.validateNeuralConnectivity();
        assert(connectivityValid);
        
        // Test percept validation
        SensoryPercept percept = createTestPercept();
        bool perceptValid = agentBrain.validatePercept(percept);
        assert(perceptValid);
        
        std::cout << "  PASSED" << std::endl;
    }
};

int main() {
    try {
        AgentBrainTest::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}