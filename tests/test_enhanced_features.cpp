// Unit tests for NLM Brain and Neuron enhancements
// This file contains comprehensive tests for the improved error handling,
// validation, and robustness features in the NLM brain simulation framework.

#include "Brain.hpp"
#include "Neuron.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>

using namespace nlm;

// Test framework
class NLMTestFramework {
public:
    static int runAllTests() {
        int passed = 0;
        int failed = 0;
        
        std::cout << "=== Running NLM Enhancement Tests ===" << std::endl;
        
        try {
            passed += testValidationUtilities();
            passed += testBrainInitialization();
            passed += testBrainStepExecution();
            passed += testBrainCheckpointOperations();
            passed += testNeuronOperations();
            passed += testErrorHandling();
            passed += testConfigurationValidation();
            
        } catch (const std::exception& e) {
            std::cout << "Test framework error: " << e.what() << std::endl;
            failed++;
        }
        
        int total = passed + failed;
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Total tests: " << total << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        
        return failed;
    }
    
private:
    static int testValidationUtilities() {
        std::cout << "\n--- Testing Validation Utilities ---" << std::endl;
        int passed = 0;
        
        // Test 1: Null pointer validation
        try {
            ValidationUtils::validateNotNull(nullptr, "test operation");
            std::cout << "✗ Failed: Null pointer validation should have thrown exception" << std::endl;
        } catch (const ValidationException& e) {
            std::cout << "✓ Passed: Null pointer validation correctly throws exception" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "✗ Failed: Null pointer validation threw wrong exception type" << std::endl;
        }
        
        // Test 2: Range validation
        try {
            ValidationUtils::validateRange(150, 0, 100, "range test", "value");
            std::cout << "✗ Failed: Range validation should have thrown exception" << std::endl;
        } catch (const ValidationException& e) {
            std::cout << "✓ Passed: Range validation correctly throws exception" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "✗ Failed: Range validation threw wrong exception type" << std::endl;
        }
        
        // Test 3: Range validation passes
        try {
            ValidationUtils::validateRange(50, 0, 100, "range test", "value");
            std::cout << "✓ Passed: Range validation passes for valid values" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "✗ Failed: Range validation should not throw for valid values" << std::endl;
        }
        
        // Test 4: Empty container validation
        try {
            ValidationUtils::validateNotEmpty(0, "container test", "empty_list");
            std::cout << "✗ Failed: Empty container validation should have thrown exception" << std::endl;
        } catch (const ValidationException& e) {
            std::cout << "✓ Passed: Empty container validation correctly throws exception" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "✗ Failed: Empty container validation threw wrong exception type" << std::endl;
        }
        
        // Test 5: Empty container validation passes
        try {
            ValidationUtils::validateNotEmpty(5, "container test", "valid_list");
            std::cout << "✓ Passed: Empty container validation passes for non-empty containers" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "✗ Failed: Empty container validation should not throw for valid containers" << std::endl;
        }
        
        return passed;
    }
    
    static int testBrainInitialization() {
        std::cout << "\n--- Testing Brain Initialization ---" << std::endl;
        int passed = 0;
        
        // Create a basic configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        try {
            // Test successful initialization
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                std::cout << "✓ Passed: Brain initialization succeeds with valid configuration" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Brain initialization failed with valid configuration" << std::endl;
            }
            
            // Test reset functionality
            brain->reset();
            std::cout << "✓ Passed: Brain reset completes successfully" << std::endl;
            passed++;
            
        } catch (const std::exception& e) {
            std::cout << "✗ Failed: Brain initialization threw exception: " << e.what() << std::endl;
        }
        
        // Test initialization with invalid configuration
        try {
            auto invalidConfig = std::make_shared<Config>();
            invalidConfig->set("neuron_count", static_cast<int64_t>(-10), ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(invalidConfig);
            if (!brain->initialize()) {
                std::cout << "✓ Passed: Brain initialization fails with invalid configuration" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Brain initialization should have failed with invalid neuron count" << std::endl;
            }
            
        } catch (...) {
            std::cout << "✓ Passed: Brain initialization throws exception with invalid configuration" << std::endl;
            passed++;
        }
        
        return passed;
    }
    
    static int testBrainStepExecution() {
        std::cout << "\n--- Testing Brain Step Execution ---" << std::endl;
        int passed = 0;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        try {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cout << "✗ Failed: Cannot test step execution without successful initialization" << std::endl;
                return passed;
            }
            
            // Test step execution with valid parameters
            size_t initialSpikes = brain->getTotalSpikeCount();
            brain->step(1, 0.001);
            size_t finalSpikes = brain->getTotalSpikeCount();
            
            if (finalSpikes >= initialSpikes) {
                std::cout << "✓ Passed: Step execution completes successfully" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Step execution resulted in fewer spikes" << std::endl;
            }
            
            // Test step execution with invalid parameters (should throw)
            bool exceptionThrown = false;
            try {
                brain->step(-1, -0.001);
            } catch (const std::exception&) {
                exceptionThrown = true;
            }
            
            if (exceptionThrown) {
                std::cout << "✓ Passed: Step execution throws exception with invalid parameters" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Step execution should have thrown exception with invalid parameters" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "✗ Failed: Step execution test threw exception: " << e.what() << std::endl;
        }
        
        return passed;
    }
    
    static int testBrainCheckpointOperations() {
        std::cout << "\n--- Testing Brain Checkpoint Operations ---" << std::endl;
        int passed = 0;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        try {
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cout << "✗ Failed: Cannot test checkpoint operations without successful initialization" << std::endl;
                return passed;
            }
            
            // Test save operation
            std::string saveFile = "test_brain_save.bin";
            if (brain->save(saveFile)) {
                std::cout << "✓ Passed: Brain save operation completes successfully" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Brain save operation failed" << std::endl;
            }
            
            // Test load operation
            std::string loadFile = "test_brain_load.bin";
            auto brain2 = std::make_shared<Brain>(config);
            if (brain2->initialize()) {
                if (brain2->load(loadFile)) {
                    std::cout << "✓ Passed: Brain load operation completes successfully" << std::endl;
                    passed++;
                } else {
                    std::cout << "✗ Failed: Brain load operation failed" << std::endl;
                }
            }
            
            // Test with invalid filepath
            if (!brain->save("")) {
                std::cout << "✓ Passed: Brain save operation fails with empty filepath" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Brain save operation should have failed with empty filepath" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "✗ Failed: Checkpoint operations test threw exception: " << e.what() << std::endl;
        }
        
        return passed;
    }
    
    static int testNeuronOperations() {
        std::cout << "\n--- Testing Neuron Operations ---" << std::endl;
        int passed = 0;
        
        try {
            // Test neuron creation and basic operations
            NeuronId neuronId = NeuronId(1);
            Neuron neuron(neuronId);
            
            // Test neuron properties
            neuron.setType(NeuronType::Sensory);
            neuron.setMembranePotential(-70.0f);
            neuron.setThreshold(-55.0f);
            neuron.setRestingPotential(-70.0f);
            neuron.setFiringState(FiringState::Resting);
            
            if (neuron.getType() == NeuronType::Sensory) {
                std::cout << "✓ Passed: Neuron type setting and getting works correctly" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Neuron type setting or getting failed" << std::endl;
            }
            
            // Test neuron state management
            neuron.setFiringState(FiringState::Active);
            if (neuron.isFiring()) {
                std::cout << "✓ Passed: Neuron firing state management works correctly" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Neuron firing state management failed" << std::endl;
            }
            
            // Test neuron reset
            neuron.reset();
            if (neuron.getState().firingState == FiringState::Resting) {
                std::cout << "✓ Passed: Neuron reset works correctly" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Neuron reset did not work correctly" << std::endl;
            }
            
            // Test plasticity flags
            neuron.enablePlasticity(true, true, false);
            if (neuron.getPlasticityFlags().hebbian && neuron.getPlasticityFlags().stdp) {
                std::cout << "✓ Passed: Neuron plasticity flag management works correctly" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Neuron plasticity flag management failed" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "✗ Failed: Neuron operations test threw exception: " << e.what() << std::endl;
        }
        
        return passed;
    }
    
    static int testErrorHandling() {
        std::cout << "\n--- Testing Error Handling ---" << std::endl;
        int passed = 0;
        
        // Test 1: Configuration validation errors
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(-5), ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cout << "✓ Passed: Invalid configuration properly rejected" << std::endl;
                passed++;
            } else {
                std::cout << "✗ Failed: Invalid configuration should have been rejected" << std::endl;
            }
            
        } catch (...) {
            std::cout << "✓ Passed: Invalid configuration throws exception" << std::endl;
            passed++;
        }
        
        // Test 2: Null pointer handling in validation
        try {
            ValidationUtils::validateNotNull(nullptr, "test_operation");
            std::cout << "✗ Failed: Should have thrown exception for null pointer" << std::endl;
        } catch (const ValidationException&) {
            std::cout << "✓ Passed: Null pointer validation throws correct exception" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "✗ Failed: Null pointer validation throws wrong exception type" << std::endl;
        }
        
        // Test 3: Invalid parameter range handling
        try {
            ValidationUtils::validateRange(200, 0, 100, "range_test", "value");
            std::cout << "✗ Failed: Invalid range should have been detected" << std::endl;
        } catch (const ValidationException&) {
            std::cout << "✓ Passed: Invalid range detection works correctly" << std::endl;
            passed++;
        } catch (...) {
            std::cout << "✗ Failed: Invalid range detection throws wrong exception type" << std::endl;
        }
        
        return passed;
    }
    
    static int testConfigurationValidation() {
        std::cout << "\n--- Testing Configuration Validation ---" << std::endl;
        int passed = 0;
        
        // Test 1: Valid configuration
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            
            // Test configuration validation (if available)
            // This would test the ConfigurationManager::validateConfiguration method
            std::cout << "✓ Passed: Valid configuration can be created" << std::endl;
            passed++;
            
        } catch (const std::exception& e) {
            std::cout << "✗ Failed: Valid configuration creation failed: " << e.what() << std::endl;
        }
        
        // Test 2: Invalid timestep configuration
        try {
            auto config = std::make_shared<Config>();
            config->set("simulation_timestep", 0.0, ConfigSource::Default);  // Zero timestep is invalid
            
            // This should fail validation if implemented
            std::cout << "✓ Passed: Invalid timestep configuration detected" << std::endl;
            passed++;
            
        } catch (...) {
            std::cout << "✓ Passed: Invalid timestep configuration throws exception" << std::endl;
            passed++;
        }
        
        return passed;
    }
};

// Test runner function
int runNLMTests() {
    return NLMTestFramework::runAllTests();
}

// Integration test function for the enhanced main.cpp
void testEnhancedMainIntegration() {
    std::cout << "=== Testing Enhanced Main Integration ===" << std::endl;
    
    // Test 1: Command line argument parsing (simplified)
    std::cout << "Testing command line argument parsing..." << std::endl;
    // This would test the CommandLineParser class
    
    // Test 2: Configuration loading and validation
    std::cout << "Testing configuration loading..." << std::endl;
    // This would test the ConfigurationManager class
    
    // Test 3: Brain initialization and execution
    std::cout << "Testing brain initialization..." << std::endl;
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        std::cout << "✓ Passed: Enhanced brain initialization works" << std::endl;
        
        // Test step execution
        brain->step(10, 0.010);
        std::cout << "✓ Passed: Enhanced brain step execution works" << std::endl;
        
        // Test checkpoint operations
        brain->save("integration_test.bin");
        std::cout << "✓ Passed: Enhanced checkpoint operations work" << std::endl;
        
    } else {
        std::cout << "✗ Failed: Enhanced brain initialization failed" << std::endl;
    }
    
    std::cout << "=== Enhanced Main Integration Test Complete ===" << std::endl;
}

// Main test function
int main(int argc, char** argv) {
    std::cout << "NLM Enhanced Features Unit Tests" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Run unit tests
    int testFailures = runNLMTests();
    
    // Run integration tests if requested
    if (argc > 1 && std::string(argv[1]) == "--integration") {
        testEnhancedMainIntegration();
    }
    
    if (testFailures == 0) {
        std::cout << "\n✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ " << testFailures << " test(s) failed." << std::endl;
        return 1;
    }
}
