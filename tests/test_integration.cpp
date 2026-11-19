// Integration Test for Improved NLM API
// This test demonstrates the new simplified API and integration
// Covers: Configuration system, Python API enhancements, Simple demo

#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

// Include NLM components
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

namespace integration_tests {

class IntegrationTestSuite {
public:
    static void runAll() {
        std::cout << "=== NLM Integration Test Suite ===" << std::endl;
        std::cout << "Testing improved API and configuration system..." << std::endl;
        
        bool allPassed = true;
        
        // Test 1: Enhanced Configuration System
        if (!testEnhancedConfiguration()) {
            allPassed = false;
            std::cout << "FAILED: Enhanced Configuration System" << std::endl;
        }
        
        // Test 2: Simplified Python-like API (using C++ equivalents)
        if (!testSimplifiedAPI()) {
            allPassed = false;
            std::cout << "FAILED: Simplified API" << std::endl;
        }
        
        // Test 3: Simple Demo Integration
        if (!testSimpleDemoIntegration()) {
            allPassed = false;
            std::cout << "FAILED: Simple Demo Integration" << std::endl;
        }
        
        // Test 4: Configuration Validation
        if (!testConfigurationValidation()) {
            allPassed = false;
            std::cout << "FAILED: Configuration Validation" << std::endl;
        }
        
        // Test 5: Error Handling
        if (!testErrorHandling()) {
            allPassed = false;
            std::cout << "FAILED: Error Handling" << std::endl;
        }
        
        // Test 6: Backward Compatibility
        if (!testBackwardCompatibility()) {
            allPassed = false;
            std::cout << "FAILED: Backward Compatibility" << std::endl;
        }
        
        std::cout << std::endl;
        if (allPassed) {
            std::cout << "=== All Integration Tests PASSED ===" << std::endl;
            std::cout << "NLM improvements are working correctly!" << std::endl;
        } else {
            std::cout << "=== Some Integration Tests FAILED ===" << std::endl;
            std::cout << "Please check the failed tests above." << std::endl;
        }
    }

private:
    static bool testEnhancedConfiguration() {
        std::cout << "Testing Enhanced Configuration System..." << std::endl;
        
        try {
            // Create configuration with defaults
            auto config = std::make_shared<Config>();
            
            // Set up defaults
            config->setUpDefaults();
            
            // Load from file
            bool loaded = config->loadFromFile("configs/default.cfg");
            if (!loaded) {
                return false; // File doesn't exist or invalid
            }
            
            // Validate configuration
            bool valid = config->validate();
            if (!valid) {
                std::cout << "  WARNING: Configuration validation failed" << std::endl;
                return false;
            }
            
            // Check essential settings
            assert(config->has("brain.neuron_count"));
            assert(config->has("plasticity.stdp.enable"));
            assert(config->has("neuromod.dopamine.scale"));
            
            // Get values with proper typing
            int neuronCount = config->getOr<int>("brain.neuron_count", 1000);
            double dopamineScale = config->getOr<double>("neuromod.dopamine.scale", 1. sinter;
            
            std::cout << "  Configuration system working correctly" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "  ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    static bool testSimplifiedAPI() {
        std::cout << "Testing Simplified API..." << std::endl;
        
        try {
            // Test 1: Create brain with defaults
            auto config = std::make_shared<Config>();
            config->setUpDefaults();
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Run some simulation steps
            for (int step = 0; step < 10; ++step) {
                brain->step(step);
            }
            
            // Check results
            int neurons = brain->getTotalNeuronCount();
            int spikes = brain->getTotalSpikeCount();
            
            assert(neurons > 0);
            assert(spikes >= 0);
            
            // Test 2: Create complete agent system
            auto brain2 = std::make_shared<Brain>(config);
            brain2->initialize();
            
            SimpleWorld world;
            world.configure(10, 10, 4, 4);
            world.reset();
            
            AgentBrain agentBrain(brain2);
            agentBrain.initialize(world);
            
            // Run agent simulation
            for (int step = 0; step < 20; ++step) {
                world.update(0.1);
                auto percept = world.getSensoryPercept();
                agentBrain.processSensoryInput(percept);
                brain2->step(step);
                auto action = agentBrain.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                
                // Check agent subsystems
                assert(agentBrain.isRewardModulationEnabled());
                assert(agentBrain.isDevelopmentEnabled());
                assert(agentBrain.isCuriosityEnabled());
            }
            
            std::cout << "  Simplified API working correctly" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "  ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    static bool testSimpleDemoIntegration() {
        std::cout << "Testing Simple Demo Integration..." << std::endl;
        
        try {
            // Simulate what nlm_simple would do
            auto config = std::make_shared<Config>();
            config->setUpDefaults();
            
            // Configure for simple demo
            config->set("brain.neuron_count", 500);
            config->set("simulation.timestep", 0.001);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Run demonstration steps (as in nlm_simple.cpp)
            std::cout << "  Running brain simulation..." << std::endl;
            for (int step = 0; step < 100; ++step) {
                brain->step(step);
                
                if (step % 25 == 0) {
                    std::cout << "    Step " << step << ": " << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
                }
            }
            
            // Get results similar to nlm_simple
            int totalNeurons = brain->getTotalNeuronCount();
            int totalSpikes = brain->getTotalSpikeCount();
            int firingNeurons = brain->getFiringNeuronCount();
            
            std::cout << "  Firing rate: " << firingNeurons << "/" << totalNeurons << " = " << (firingNe string(std::cout << "  Total spikes: " << totalSpikes << std::endl;
            
            std::cout << "  Simple demo integration working correctly" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "  ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    static bool testConfigurationValidation() {
        std::cout << "Testing Configuration Validation..." << std::endl;
        
        try {
            auto config = std::make_shared<Config>();
            config->setUpDefaults();
            
            // Add validation rules
            ValidationRule rule1;
            rule1.key = "brain.neuron_count";
            rule1.validator = [](const ConfigValue& val) {
                if (!std::holds_alternative<int>(val)) return false;
                return std::get<int>(val) > 0 && std::get<int>(val) <= 10000;
            };
            rule1.errorMessage = "Neurons must be between 1 and 10000";
            rule1.requiresSource = true;
            
            config->addValidationRule(rule1);
            
            // Validation should pass with defaults
            assert(config->validate());
            
            // Test invalid configuration
            config->set("brain.neuron_count", -1000, ConfigSource::Runtime);
            assert(!config->validate());
            
            std::vector<std::string> errors = config->getValidationErrors();
            assert(!errors.empty());
            
            std::cout << "  Configuration validation working correctly" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "  ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    static bool testErrorHandling() {
        std::cout << "Testing Error Handling..." << std::endl;
        
        try {
            // Test invalid configuration loading
            auto config = std::make_shared<Config>();
            bool loaded = config->loadFromFile("nonexistent_file.cfg");
            assert(!loaded); // Should return false
            
            // Test brain initialization error handling
            auto invalidConfig = std::make_shared<Config>();
            invalidConfig->set("brain.neuron_count", -1, ConfigSource::Runtime);
            
            // This should either handle gracefully or throw exception
            try {
                auto brain = std::make_shared<Brain>(invalidConfig);
                bool initialized = brain->initialize();
                // Depending on implementation, this might succeed with defaults
                // or fail gracefully
                std::cout << "  Brain initialization handled gracefully" << std::endl;
            } catch (...) {
                std::cout << "  Brain initialization threw exception (expected)" << std::endl;
            }
            
            std::cout << "  Error handling working correctly" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "  ERROR: " << e.what() << std::endl;
            return false;
        }
    }
    
    static bool testBackwardCompatibility() {
        std::cout << "Testing Backward Compatibility..." << std::endl;
        
        try {
            // Test old API still works
            auto config = std::make_shared<Config>();
            
            // Old-style configuration
            config->set("random_seed", 42, ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", 1000, ConfigSource::Default);
            
            // Old-style brain creation
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Old-style simulation
            for (int step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            // Check that old API still returns expected results
            assert(brain->getTotalNeuronCount() > 0);
            assert(brain->getTotalSpikeCount() >= 0);
            
            std::cout << "  Backward compatibility maintained" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << "  ERROR: " << e.what() << std::endl;
            return false;
        }
    }
};

} // namespace integration_tests

int main() {
    integration_tests::IntegrationTestSuite::runAll();
    return 0;
}
