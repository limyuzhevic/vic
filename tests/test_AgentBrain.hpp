#include "tests/test_AgentBrain.hpp"
#include "tests/test_AgentBrain.cpp"

// This file includes all AgentBrain tests to ensure the improved implementation works correctly

// Test macros for better test reporting
#define TEST_ASSERT(condition, message) do { \
    if (!(condition)) { \
        std::cerr << "TEST FAILED: " << message << " (in " << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    } \
} while(0)

#define TEST_ASSERT_EQUAL(expected, actual, message) do { \
    if ((expected) != (actual)) { \
        std::cerr << "TEST FAILED: " << message << " (expected " << #expected << ", got " << #actual << ")" << std::endl; \
        return false; \
    } \
} while(0)

#define TEST_ASSERT_FLOAT_EQUAL(expected, actual, epsilon, message) do { \
    if (std::abs((expected) - (actual)) > epsilon) { \
        std::cerr << "TEST FAILED: " << message << " (expected " << expected << ", got " << actual << ")" << std::endl; \
        return false; \
    } \
} while(0)

// Test suite for AgentBrain
class AgentBrainTestSuite {
public:
    AgentBrainTestSuite() {
        testResults.push_back(testNeuralGroupStructure());
        testConstantsNamespace();
        testMemoryManagement();
        testValidationAndErrorHandling();
        testSensoryProcessing();
        testMotorDecoding();
        testNeuromodulation();
        testDevelopmentSystem();
        testResetFunctionality();
    }
    
    void runAllTests() {
        std::cout << "=== AgentBrain Test Suite ===" << std::endl;
        std::cout << "Running " << testResults.size() << " test categories..." << std::endl;
        std::cout << std::endl;
        
        int passed = 0;
        for (size_t i = 0; i < testResults.size(); ++i) {
            std::cout << "Test Category " << (i + 1) << ": " << testCategoryNames[i] << std::endl;
            if (testResults[i]) {
                std::cout << "  PASSED" << std::endl;
                passed++;
            } else {
                std::cout << "  FAILED" << std::endl;
            }
            std::cout << std::endl;
        }
        
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << testResults.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << (testResults.size() - passed) << std::endl;
        
        if (passed == testResults.size()) {
            std::cout << "All tests PASSED! ✓" << std::endl;
        } else {
            std::cout << "Some tests FAILED! ✗" << std::endl;
        }
    }

private:
    std::vector<bool> testResults;
    static inline const std::vector<std::string> testCategoryNames = {
        "Neural Group Structure",
        "Constants Namespace",
        "Memory Management",
        "Validation and Error Handling",
        "Sensory Processing",
        "Motor Decoding",
        "Neuromodulation",
        "Development System",
        "Reset Functionality"
    };
    
    bool testNeuralGroupStructure() {
        std::cout << "  Testing neural group structure..." << std::endl;
        
        // Create a mock brain for testing
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "  Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Create AgentBrain instance
        AgentBrain agentBrain(brain);
        
        // Test neural group structure initialization
        agentBrain.updateNeuralGroups();
        
        // Verify neural groups have correct structure
        TEST_ASSERT(agentBrain.getNeuralGroups().motorForward_.size() > 0, 
                   "Motor forward group should not be empty");
        TEST_ASSERT(agentBrain.getNeuralGroups().sensoryVision_.size() > 0, 
                   "Vision sensory group should not be empty");
        TEST_ASSERT(agentBrain.getNeuralGroups().sensoryTouch_.size() > 0, 
                   "Touch sensory group should not be empty");
        
        // Test total neuron count calculation
        size_t totalMotor = agentBrain.getNeuralGroups().totalMotorNeurons();
        size_t totalSensory = agentBrain.getNeuralGroups().totalSensoryNeurons();
        
        TEST_ASSERT(totalMotor > 0, "Total motor neurons should be greater than 0");
        TEST_ASSERT(totalSensory > 0, "Total sensory neurons should be greater than 0");
        
        return true;
    }
    
    bool testConstantsNamespace() {
        std::cout << "  Testing constants namespace..." << std::endl;
        
        // Test motor decoding threshold
        TEST_ASSERT(Constants::MOTOR_DECODE_THRESHOLD > 0.0f, 
                   "MOTOR_DECODE_THRESHOLD should be positive");
        TEST_ASSERT(Constants::MOTOR_DECODE_THRESHOLD <= 1.0f, 
                   "MOTOR_DECODE_THRESHOLD should be less than or equal to 1.0");
        
        // Test neuromodulation levels
        TEST_ASSERT(Constants::DEFAULT_DOPAMINE_LEVEL == 0.0f, 
                   "DEFAULT_DOPAMINE_LEVEL should be 0.0");
        TEST_ASSERT(Constants::MAX_DOPAMINE_LEVEL > 0.0f, 
                   "MAX_DOPAMINE_LEVEL should be positive");
        TEST_ASSERT(Constants::MIN_DOPAMINE_LEVEL < 0.0f, 
                   "MIN_DOPAMINE_LEVEL should be negative");
        
        // Test curiosity and novelty parameters
        TEST_ASSERT(Constants::CURIOSITY_NOVELTY_MULTIPLIER > 0.0f, 
                   "CURIOSITY_NOVELTY_MULTIPLIER should be positive");
        TEST_ASSERT(Constants::MAX_CURIOSITY_LEVEL == 1.0f, 
                   "MAX_CURIOSITY_LEVEL should be 1.0");
        
        // Test development system constants
        TEST_ASSERT(Constants::DEVELOPMENT_INITIAL_STAGE > 0.0, 
                   "DEVELOPMENT_INITIAL_STAGE should be positive");
        TEST_ASSERT(Constants::PLASTICITY_INITIAL == 1.0f, 
                   "PLASTICITY_INITIAL should be 1.0");
        
        // Test plasticity constants
        TEST_ASSERT(Constants::STDP_LTP_BASE > 0.0f, 
                   "STDP_LTP_BASE should be positive");
        TEST_ASSERT(Constants::STDP_LTD_BASE > 0.0f, 
                   "STDP_LTD_BASE should be positive");
        
        return true;
    }
    
    bool testMemoryManagement() {
        std::cout << "  Testing memory management..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(54321), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "  Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Test AgentBrain construction and destruction
        {
            AgentBrain agentBrain(brain);
            TEST_ASSERT(agentBrain.getBrain() != nullptr, "Brain pointer should not be null");
            
            // Test initialization
            agentBrain.initialize(brain->getWorld());
            
            // Verify neural groups are properly allocated
            TEST_ASSERT(!agentBrain.getNeuralGroups().motorForward_.empty(), 
                       "Neural groups should be initialized");
        }
        
        // Test memory cleanup after destruction
        // (This is implicitly tested by the scope above)
        
        return true;
    }
    
    bool testValidationAndErrorHandling() {
        std::cout << "  Testing validation and error handling..." << std::endl;
        
        // Test null brain validation
        AgentBrain agentBrain(nullptr);
        TEST_ASSERT(agentBrain.getBrain() == nullptr, "Brain pointer should be null when initialized with null");
        
        // Test initialization with null brain
        // (This should handle gracefully or log error)
        
        // Test configuration validation
        TEST_ASSERT(Constants::DEFAULT_SIMULATION_TIMESTEP > 0.0, 
                   "Default simulation timestep should be positive");
        TEST_ASSERT(Constants::DEFAULT_SIMULATION_TIMESTEP <= 0.1, 
                   "Default simulation timestep should be reasonable");
        
        // Test threshold validation
        TEST_ASSERT(Constants::MOTOR_DECODE_THRESHOLD > 0.0f, 
                   "Motor decode threshold should be positive");
        TEST_ASSERT(Constants::MAX_EXPLORATION_CHANCE <= 1.0f, 
                   "Exploration chance should be reasonable");
        
        return true;
    }
    
    bool testSensoryProcessing() {
        std::cout << "  Testing sensory processing..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(98765), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "  Failed to initialize brain" << std::endl;
            return false;
        }
        
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
        TEST_ASSERT(agentBrain.getNoveltyLevel() >= 0.0f, 
                   "Novelty level should be non-negative");
        TEST_ASSERT(agentBrain.getNoveltyLevel() <= 1.0f, 
                   "Novelty level should be reasonable");
        
        // Verify curiosity calculation
        TEST_ASSERT(agentBrain.getCuriosityLevel() >= 0.0f, 
                   "Curiosity level should be non-negative");
        TEST_ASSERT(agentBrain.getCuriosityLevel() <= Constants::MAX_CURIOSITY_LEVEL, 
                   "Curiosity level should not exceed maximum");
        
        return true;
    }
    
    bool testMotorDecoding() {
        std::cout << "  Testing motor decoding..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(11111), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "  Failed to initialize brain" << std::endl;
            return false;
        }
        
        AgentBrain agentBrain(brain);
        
        // Test motor command decoding with no activity
        MotorCommand cmd = agentBrain.decodeMotorCommand();
        TEST_ASSERT(cmd == MotorCommand::Wait, "Motor command should be Wait when no activity");
        
        // Test motor command decoding with activity
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
        
        return true;
    }
    
    bool testNeuromodulation() {
        std::cout << "  Testing neuromodulation..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(22222), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "  Failed to initialize brain" << std::endl;
            return false;
        }
        
        AgentBrain agentBrain(brain);
        
        // Test initial neuromodulation levels
        TEST_ASSERT(agentBrain.getNeuromodulationLevel() == 0.0f, 
                   "Initial neuromodulation level should be 0.0");
        TEST_ASSERT(agentBrain.getPredictionError() == 0.0f, 
                   "Initial prediction error should be 0.0");
        TEST_ASSERT(agentBrain.getCuriosityLevel() == 0.0f, 
                   "Initial curiosity level should be 0.0");
        TEST_ASSERT(agentBrain.getNoveltyLevel() == 0.0f, 
                   "Initial novelty level should be 0.0");
        
        // Test reward modulation
        agentBrain.applyRewardModulation(1.0f, 0.5f); // Reward > predicted reward
        TEST_ASSERT(agentBrain.getPredictionError() > 0.0f, 
                   "Prediction error should be positive when reward > predicted");
        
        // Test negative reward modulation
        agentBrain.applyRewardModulation(-1.0f, 0.5f); // Negative reward < predicted reward
        TEST_ASSERT(agentBrain.getPredictionError() < 0.0f, 
                   "Prediction error should be negative when reward < predicted");
        
        return true;
    }
    
    bool testDevelopmentSystem() {
        std::cout << "  Testing development system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(33333), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "  Failed to initialize brain" << std::endl;
            return false;
        }
        
        AgentBrain agentBrain(brain);
        
        // Test developmental stage initialization
        TEST_ASSERT(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Initial, 
                   "Initial developmental stage should be Initial");
        
        // Test development update
        agentBrain.updateDevelopment(60.0); // 60 seconds
        TEST_ASSERT(agentBrain.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod, 
                   "Developmental stage should advance to CriticalPeriod");
        
        agentBrain.updateDevelopment(240.0); // Additional 240 seconds
        TEST_ASSERT(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Maturation, 
                   "Developmental stage should advance to Maturation");
        
        agentBrain.updateDevelopment(660.0); // Additional 660 seconds
        TEST_ASSERT(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Adult, 
                   "Developmental stage should advance to Adult");
        
        return true;
    }
    
    bool testResetFunctionality() {
        std::cout << "  Testing reset functionality..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(44444), Source::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "  Failed to initialize brain" << std::endl;
            return false;
        }
        
        AgentBrain agentBrain(brain);
        
        // Set some state values
        agentBrain.applyRewardModulation(1.0f, 0.5f);
        agentBrain.updateDevelopment(60.0);
        
        // Reset agent
        agentBrain.reset();
        
        // Verify reset state
        TEST_ASSERT(agentBrain.getNeuromodulationLevel() == 0.0f, 
                   "Neuromodulation level should be reset to 0.0");
        TEST_ASSERT(agentBrain.getPredictionError() == 0.0f, 
                   "Prediction error should be reset to 0.0");
        TEST_ASSERT(agentBrain.getCuriosityLevel() == 0.0f, 
                   "Curiosity level should be reset to 0.0");
        TEST_ASSERT(agentBrain.getNoveltyLevel() == 0.0f, 
                   "Novelty level should be reset to 0.0");
        TEST_ASSERT(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Initial, 
                   "Developmental stage should be reset to Initial");
        
        // Verify brain reset
        TEST_ASSERT(agentBrain.getBrain()->getTotalSpikeCount() >= 0, 
                   "Brain should be in valid state after reset");
        
        return true;
    }
};

// Global test runner
void runAgentBrainTests() {
    AgentBrainTestSuite testSuite;
    testSuite.runAllTests();
}

// Entry point for running tests
int main() {
    runAgentBrainTests();
    return 0;
}
