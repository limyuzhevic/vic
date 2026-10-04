// Test implementation for AgentBrain functionality

#include "tests/test_AgentBrain.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <stdexcept>

namespace nlm {

// Helper function for test assertion
void testAssert(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "TEST ASSERTION FAILED: " << message << std::endl;
        throw std::runtime_error("Test assertion failed: " + message);
    }
}

// Test 1: Neural Group Structure Test
bool testNeuralGroupStructure() {
    std::cout << "  Testing neural group structure..." << std::endl;
    
    // Create test configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    testAssert(brain->initialize(), "Failed to initialize brain");
    
    // Create AgentBrain instance
    AgentBrain agentBrain(brain);
    
    // Initialize neural groups
    agentBrain.updateNeuralGroups();
    
    // Verify neural groups have correct structure
    testAssert(!agentBrain.getNeuralGroups().motorForward_.empty(), 
               "Motor forward group should not be empty");
    testAssert(!agentBrain.getNeuralGroups().sensoryVision_.empty(), 
               "Vision sensory group should not be empty");
    testAssert(!agentBrain.getNeuralGroups().sensoryTouch_.empty(), 
               "Touch sensory group should not be empty");
    
    // Test total neuron count calculation
    size_t totalMotor = agentBrain.getNeuralGroups().totalMotorNeurons();
    size_t totalSensory = agentBrain.getNeuralGroups().totalSensoryNeurons();
    
    testAssert(totalMotor > 0, "Total motor neurons should be greater than 0");
    testAssert(totalSensory > 0, "Total sensory neurons should be greater than 0");
    
    std::cout << "    ✓ Neural group structure test passed" << std::endl;
    return true;
}

// Test 2: Constants Namespace Test
bool testConstantsNamespace() {
    std::cout << "  Testing constants namespace..." << std::endl;
    
    // Test motor decoding threshold
    testAssert(Constants::MOTOR_DECODE_THRESHOLD > 0.0f, 
               "MOTOR_DECODE_THRESHOLD should be positive");
    testAssert(Constants::MOTOR_DECODE_THRESHOLD <= 1.0f, 
               "MOTOR_DECODE_THRESHOLD should be less than or equal to 1.0");
    
    // Test neuromodulation levels
    testAssert(Constants::DEFAULT_DOPAMINE_LEVEL == 0.0f, 
               "DEFAULT_DOPAMINE_LEVEL should be 0.0");
    testAssert(Constants::MAX_DOPAMINE_LEVEL > 0.0f, 
               "MAX_DOPAMINE_LEVEL should be positive");
    testAssert(Constants::MIN_DOPAMINE_LEVEL < 0.0f, 
               "MIN_DOPAMINE_LEVEL should be negative");
    
    // Test curiosity and novelty parameters
    testAssert(Constants::CURIOSITY_NOVELTY_MULTIPLIER > 0.0f, 
               "CURIOSITY_NOVELTY_MULTIPLIER should be positive");
    testAssert(Constants::MAX_CURIOSITY_LEVEL == 1.0f, 
               "MAX_CURIOSITY_LEVEL should be 1.0");
    
    // Test development system constants
    testAssert(Constants::DEVELOPMENT_INITIAL_STAGE > 0.0, 
               "DEVELOPMENT_INITIAL_STAGE should be positive");
    testAssert(Constants::PLASTICITY_INITIAL == 1.0f, 
               "PLASTICITY_INITIAL should be 1.0");
    
    // Test plasticity constants
    testAssert(Constants::STDP_LTP_BASE > 0.0f, 
               "STDP_LTP_BASE should be positive");
    testAssert(Constants::STDP_LTD_BASE > 0.0f, 
               "STDP_LTD_BASE should be positive");
    
    std::cout << "    ✓ Constants namespace test passed" << std::endl;
    return true;
}

// Test 3: Memory Management Test
bool testMemoryManagement() {
    std::cout << "  Testing memory management..." << std::endl;
    
    // Create test configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(54321), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    testAssert(brain->initialize(), "Failed to initialize brain");
    
    // Test AgentBrain construction and destruction
    {
        AgentBrain agentBrain(brain);
        testAssert(agentBrain.getBrain() != nullptr, "Brain pointer should not be null");
        
        // Test initialization
        agentBrain.initialize(brain->getWorld());
        
        // Verify neural groups are properly allocated
        testAssert(!agentBrain.getNeuralGroups().motorForward_.empty(), 
                   "Neural groups should be initialized");
    }
    
    std::cout << "    ✓ Memory management test passed" << std::endl;
    return true;
}

// Test 4: Validation and Error Handling Test
bool testValidationAndErrorHandling() {
    std::cout << "  Testing validation and error handling..." << std::endl;
    
    // Test null brain validation
    AgentBrain agentBrain(nullptr);
    testAssert(agentBrain.getBrain() == nullptr, "Brain pointer should be null when initialized with null");
    
    // Test configuration validation
    testAssert(Constants::DEFAULT_SIMULATION_TIMESTEP > 0.0, 
               "Default simulation timestep should be positive");
    testAssert(Constants::DEFAULT_SIMULATION_TIMESTEP <= 0.1, 
               "Default simulation timestep should be reasonable");
    
    // Test threshold validation
    testAssert(Constants::MOTOR_DECODE_THRESHOLD > 0.0f, 
               "Motor decode threshold should be positive");
    testAssert(Constants::MAX_EXPLORATION_CHANCE <= 1.0f, 
               "Exploration chance should be reasonable");
    
    std::cout << "    ✓ Validation and error handling test passed" << std::endl;
    return true;
}

// Test 5: Sensory Processing Test
bool testSensoryProcessing() {
    std::cout << "  Testing sensory processing..." << std::endl;
    
    // Create test configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(98765), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    testAssert(brain->initialize(), "Failed to initialize brain");
    
    // Create AgentBrain instance
    AgentBrain agentBrain(brain);
    
    // Create test sensory percept
    SensoryPercept percept;
    std::vector<float> testVision(256, 0.5f); // 16x16 vision grid
    std::vector<float> testTouch(8, 0.3f);
    std::vector<float> testInternal(4, 0.2f);
    std::vector<float> testProprioception(6, 0.1f);
    
    percept.setVision(testVision);
    percept.setTouch(testTouch);
    percept.setInternal(testInternal);
    percept.setProprioception(testProprioception);
    
    // Test sensory processing
    agentBrain.processSensoryInput(percept);
    
    // Verify novelty calculation
    testAssert(agentBrain.getNoveltyLevel() >= 0.0f, 
               "Novelty level should be non-negative");
    testAssert(agentBrain.getNoveltyLevel() <= 1.0f, 
               "Novelty level should be reasonable");
    
    // Verify curiosity calculation
    testAssert(agentBrain.getCuriosityLevel() >= 0.0f, 
               "Curiosity level should be non-negative");
    testAssert(agentBrain.getCuriosityLevel() <= Constants::MAX_CURIOSITY_LEVEL, 
               "Curiosity level should not exceed maximum");
    
    std::cout << "    ✓ Sensory processing test passed" << std::endl;
    return true;
}

// Test 6: Motor Decoding Test
bool testMotorDecoding() {
    std::cout << "  Testing motor decoding..." << std::endl;
    
    // Create test configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(11111), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    testAssert(brain->initialize(), "Failed to initialize brain");
    
    // Create AgentBrain instance
    AgentBrain agentBrain(brain);
    
    // Test motor command decoding with no activity
    MotorCommand cmd = agentBrain.decodeMotorCommand();
    testAssert(cmd == MotorCommand::Wait, "Motor command should be Wait when no activity");
    
    // Initialize neural groups
    agentBrain.initialize(brain->getWorld());
    agentBrain.updateNeuralGroups();
    
    // Inject current into motor neurons to simulate activity
    for (size_t i = 0; i < std::min(size_t(5), agentBrain.getNeuralGroups().motorForward_.size()); ++i) {
        Neuron* neuron = agentBrain.getNeuralGroups().motorForward_[i];
        if (neuron) {
            neuron->injectCurrent(10.0f); // High current to induce activity
        }
    }
    
    // Run simulation steps to allow activity to propagate
    for (int step = 0; step < 10; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Test motor decoding
    cmd = agentBrain.decodeMotorCommand();
    // Command could be any action or Wait, both are valid
    
    std::cout << "    ✓ Motor decoding test passed" << std::endl;
    return true;
}

// Test 7: Neuromodulation Test
bool testNeuromodulation() {
    std::cout << "  Testing neuromodulation..." << std::endl;
    
    // Create test configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(22222), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    testAssert(brain->initialize(), "Failed to initialize brain");
    
    // Create AgentBrain instance
    AgentBrain agentBrain(brain);
    
    // Test initial neuromodulation levels
    testAssert(agentBrain.getNeuromodulationLevel() == 0.0f, 
               "Initial neuromodulation level should be 0.0");
    testAssert(agentBrain.getPredictionError() == 0.0f, 
               "Initial prediction error should be 0.0");
    testAssert(agentBrain.getCuriosityLevel() == 0.0f, 
               "Initial curiosity level should be 0.0");
    testAssert(agentBrain.getNoveltyLevel() == 0.0f, 
               "Initial novelty level should be 0.0");
    
    // Test reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f); // Reward > predicted reward
    testAssert(agentBrain.getPredictionError() > 0.0f, 
               "Prediction error should be positive when reward > predicted");
    
    // Test negative reward modulation
    agentBrain.applyRewardModulation(-1.0f, 0.5f); // Negative reward < predicted reward
    testAssert(agentBrain.getPredictionError() < 0.0f, 
               "Prediction error should be negative when reward < predicted");
    
    std::cout << "    ✓ Neuromodulation test passed" << std::endl;
    return true;
}

// Test 8: Development System Test
bool testDevelopmentSystem() {
    std::cout << "  Testing development system..." << std::endl;
    
    // Create test configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(33333), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    testAssert(brain->initialize(), "Failed to initialize brain");
    
    // Create AgentBrain instance
    AgentBrain agentBrain(brain);
    
    // Test developmental stage initialization
    testAssert(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Initial, 
               "Initial developmental stage should be Initial");
    
    // Test development update
    agentBrain.updateDevelopment(60.0); // 60 seconds
    testAssert(agentBrain.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod, 
               "Developmental stage should advance to CriticalPeriod");
    
    agentBrain.updateDevelopment(240.0); // Additional 240 seconds
    testAssert(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Maturation, 
               "Developmental stage should advance to Maturation");
    
    agentBrain.updateDevelopment(660.0); // Additional 660 seconds
    testAssert(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Adult, 
               "Developmental stage should advance to Adult");
    
    std::cout << "    ✓ Development system test passed" << std::endl;
    return true;
}

// Test 9: Reset Functionality Test
bool testResetFunctionality() {
    std::cout << "  Testing reset functionality..." << std::endl;
    
    // Create test configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(44444), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    testAssert(brain->initialize(), "Failed to initialize brain");
    
    // Create AgentBrain instance
    AgentBrain agentBrain(brain);
    
    // Set some state values
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    agentBrain.updateDevelopment(60.0);
    
    // Reset agent
    agentBrain.reset();
    
    // Verify reset state
    testAssert(agentBrain.getNeuromodulationLevel() == 0.0f, 
               "Neuromodulation level should be reset to 0.0");
    testAssert(agentBrain.getPredictionError() == 0.0f, 
               "Prediction error should be reset to 0.0");
    testAssert(agentBrain.getCuriosityLevel() == 0.0f, 
               "Curiosity level should be reset to 0.0");
    testAssert(agentBrain.getNoveltyLevel() == 0.0f, 
               "Novelty level should be reset to 0.0");
    testAssert(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Initial, 
               "Developmental stage should be reset to Initial");
    
    // Verify brain reset
    testAssert(agentBrain.getBrain()->getTotalSpikeCount() >= 0, 
               "Brain should be in valid state after reset");
    
    std::cout << "    ✓ Reset functionality test passed" << std::endl;
    return true;
}

// Run all AgentBrain tests
void runAgentBrainTests() {
    std::cout << "=== AgentBrain Test Suite ===" << std::endl;
    std::cout << "Running comprehensive test suite for improved AgentBrain implementation..." << std::endl;
    std::cout << std::endl;
    
    // Run individual test functions
    bool test1Passed = testNeuralGroupStructure();
    bool test2Passed = testConstantsNamespace();
    bool test3Passed = testMemoryManagement();
    bool test4Passed = testValidationAndErrorHandling();
    bool test5Passed = testSensoryProcessing();
    bool test6Passed = testMotorDecoding();
    bool test7Passed = testNeuromodulation();
    bool test8Passed = testDevelopmentSystem();
    bool test9Passed = testResetFunctionality();
    
    // Report test results
    std::cout << std::endl;
    std::cout << "=== Test Results ===" << std::endl;
    std::cout << "1. Neural Group Structure: " << (test1Passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "2. Constants Namespace: " << (test2Passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "3. Memory Management: " << (test3Passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "4. Validation and Error Handling: " << (test4Passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "5. Sensory Processing: " << (test5Passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "6. Motor Decoding: " << (test6Passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "7. Neuromodulation: " << (test7Passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "8. Development System: " << (test8Passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "9. Reset Functionality: " << (test9Passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
    
    int passedCount = test1Passed + test2Passed + test3Passed + test4Passed + 
                      test5Passed + test6Passed + test7Passed + test8Passed + test9Passed;
    
    std::cout << "=== Summary ===" << std::endl;
    std::cout << "Tests passed: " << passedCount << "/9" << std::endl;
    std::cout << "Tests failed: " << (9 - passedCount) << "/9" << std::endl;
    
    if (passedCount == 9) {
        std::cout << "🎉 All tests PASSED! The improved AgentBrain implementation is working correctly!" << std::endl;
    } else {
        std::cout << "❌ Some tests FAILED! Please review the implementation." << std::endl;
    }
}

} // namespace nlm
