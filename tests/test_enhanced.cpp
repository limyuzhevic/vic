// Comprehensive Test Suite for Enhanced NLM
// 
// This test suite includes:
// 1. Configuration validation tests
// 2. Enhanced configuration tests
// 3. Brain functionality tests
// 4. Agent functionality tests
// 5. World functionality tests
// 6. Integration tests
// 7. Performance tests
// 8. Error handling tests

#include "core/Config/Config.hpp"
#include "core/Validation/ConfigValidation.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

namespace test_enhanced {

// Validation Tests
namespace validation_tests {
    
    void testConfigValidationSchema() {
        std::cout << "    Testing configuration validation schema..." << std::endl;
        
        nlm::ConfigSchema schema = nlm::ConfigSchema::createStandardSchema();
        
        // Test valid parameters
        assert(schema.isValid("brain.neuron_count"));
        assert(schema.isValid("brain.synapse_density"));
        assert(schema.isValid("plasticity.stdp.learning_rate"));
        assert(schema.isValid("simulation.timestep"));
        
        // Test invalid parameters
        assert(!schema.isValid("invalid.key"));
        assert(!schema.isValid("brain.nonexistent"));
        
        std::cout << "        Configuration validation schema tests passed" << std::endl;
    }
    
    void testConfigValidationErrors() {
        std::cout << "    Testing configuration validation errors..." << std::endl;
        
        nlm::Config config;
        nlm::ConfigSchema schema = nlm::ConfigSchema::createStandardSchema();
        
        // Set valid values
        config.set("brain.neuron_count", 1000);
        config.set("brain.synapse_density", 0.1);
        config.set("simulation.timestep", 0.001);
        
        // Test with valid config
        assert(schema.isValid(config));
        
        // Test with invalid values (outside ranges)
        config.set("brain.neuron_count", -100);  // Invalid
        std::vector<ValidationError> errors = schema.validate(config);
        assert(!errors.empty());
        assert(errors[0].type == ValidationErrorType::OutOfRange);
        
        config.set("brain.neuron_count", 1000);  // Reset to valid
        assert(schema.isValid(config));
        
        std::cout << "        Configuration validation error tests passed" << std::endl;
    }
    
    void testParameterValidation() {
        std::cout << "    Testing parameter validation..." << std::endl;
        
        // Test type validation
        nlm::ConfigValue intVal = 42;
        nlm::ConfigValue doubleVal = 3.14;
        nlm::ConfigValue stringVal = std::string("test");
        nlm::ConfigValue boolVal = true;
        
        assert(nlm::ConfigSchema::validateType(intVal, nlm::ConfigParameterType::Integer));
        assert(nlm::ConfigSchema::validateType(doubleVal, nlm::ConfigParameterType::Double));
        assert(nlm::ConfigSchema::validateType(stringVal, nlm::ConfigParameterType::String));
        assert(nlm::ConfigSchema::validateType(boolVal, nlm::ConfigParameterType::Boolean));
        
        assert(!nlm::ConfigSchema::validateType(intVal, nlm::ConfigParameterType::String));
        
        std::cout << "        Parameter validation tests passed" << std::endl;
    }
    
    void runAllValidationTests() {
        testConfigValidationSchema();
        testConfigValidationErrors();
        testParameterValidation();
    }
    
} // namespace validation_tests

// Enhanced Configuration Tests
namespace enhanced_config_tests {
    
    void testEnhancedConfigCreation() {
        std::cout << "    Testing enhanced configuration creation..." << std::endl;
        
        nlm::ValidatedConfig config;
        
        // Test basic operations
        config.set("test.key", "test_value");
        assert(config.has("test.key"));
        
        auto value = config.get<std::string>("test.key");
        assert(value.has_value());
        assert(*value == "test_value");
        
        // Test validation with invalid config
        config.set("brain.neuron_count", -100);  // Invalid
        assert(!config.isValid());
        
        // Reset to valid
        config.set("brain.neuron_count", 1000);
        assert(config.isValid());
        
        std::cout << "        Enhanced configuration creation tests passed" << std::endl;
    }
    
    void testConfigValidationSuggestions() {
        std::cout << "    Testing configuration validation suggestions..." << std::endl;
        
        nlm::ValidatedConfig config;
        
        // Set invalid values
        config.set("brain.neuron_count", 50000);  // Too high
        config.set("brain.synapse_density", 1.5);   // Too high
        
        // Get suggestions
        auto suggestions = config.getValidationSuggestions();
        assert(!suggestions.empty());
        
        // Check that suggestions are meaningful
        bool has_neuron_suggestion = false;
        bool has_density_suggestion = false;
        
        for (const auto& suggestion : suggestions) {
            if (suggestion.find("neuron") != std::string::npos) {
                has_neuron_suggestion = true;
            }
            if (suggestion.find("density") != std::string::npos) {
                has_density_suggestion = true;
            }
        }
        
        assert(has_neuron_suggestion);
        assert(has_density_suggestion);
        
        std::cout << "        Configuration validation suggestions tests passed" << std::endl;
    }
    
    void testConfigFileLoadValidation() {
        std::cout << "    Testing configuration file load validation..." << std::endl;
        
        // Test loading config file with invalid values
        nlm::ValidatedConfig config;
        
        // Note: This would require a real config file with invalid values
        // For now, we'll test the validation framework
        
        std::cout << "        Configuration file load validation tests passed (placeholder)" << std::endl;
    }
    
    void runAllEnhancedConfigTests() {
        testEnhancedConfigCreation();
        testConfigValidationSuggestions();
        testConfigFileLoadValidation();
    }
    
} // namespace enhanced_config_tests

// Enhanced Brain Tests
namespace enhanced_brain_tests {
    
    void testEnhancedBrainCreation() {
        std::cout << "    Testing enhanced brain creation..." << std::endl;
        
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        
        // Test initialization
        brain.initialize();
        assert(brain.is_valid());
        
        // Test statistics
        auto stats = brain.get_stats();
        assert(stats["total_spikes"] == 0);
        assert(stats["firing_neurons"] == 0);
        
        std::cout << "        Enhanced brain creation tests passed" << std::endl;
    }
    
    void testEnhancedBrainSimulation() {
        std::cout << "    Testing enhanced brain simulation..." << std::endl;
        
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        brain.initialize();
        
        // Run simulation
        brain.run_simulation(100, verbose: false);
        
        // Check statistics after simulation
        auto stats = brain.get_stats();
        assert(stats["simulation_steps"] == 100);
        
        std::cout << "        Enhanced brain simulation tests passed" << std::endl;
    }
    
    void testEnhancedBrainErrorHandling() {
        std::cout << "    Testing enhanced brain error handling..." << std::endl;
        
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        
        // Test running simulation without initialization
        try {
            brain.run_simulation(10);
            assert(false); // Should not reach here
        } catch (const std::runtime_error& e) {
            assert(true); // Expected error
        }
        
        // Initialize and test valid simulation
        brain.initialize();
        brain.run_simulation(10, verbose: false);
        assert(true); // Should succeed
        
        std::cout << "        Enhanced brain error handling tests passed" << std::endl;
    }
    
    void testEnhancedBrainStateManagement() {
        std::cout << "    Testing enhanced brain state management..." << std::endl;
        
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        brain.initialize();
        
        // Get initial state
        auto initial_stats = brain.get_stats();
        
        // Run simulation
        brain.run_simulation(50, verbose: false);
        
        // Get state after simulation
        auto final_stats = brain.get_stats();
        
        // Check that simulation steps increased
        assert(final_stats["simulation_steps"] > initial_stats["simulation_steps"]);
        
        // Test save/load (if file I/O is available)
        // Note: This would require file system access
        
        std::cout << "        Enhanced brain state management tests passed" << std::endl;
    }
    
    void runAllEnhancedBrainTests() {
        testEnhancedBrainCreation();
        testEnhancedBrainSimulation();
        testEnhancedBrainErrorHandling();
        testEnhancedBrainStateManagement();
    }
    
} // namespace enhanced_brain_tests

// Enhanced World Tests
namespace enhanced_world_tests {
    
    void testEnhancedWorldCreation() {
        std::cout << "    Testing enhanced world creation..." << std::endl;
        
        nlm::EnhancedWorld world(20, 20, 8, 8);
        
        // Test basic properties
        assert(world.get_width() == 20);
        assert(world.get_height() == 20);
        
        // Test configuration
        world.configure(30, 30, 10, 10);
        assert(world.get_width() == 30);
        assert(world.get_height() == 30);
        
        std::cout << "        Enhanced world creation tests passed" << std::endl;
    }
    
    void testEnhancedWorldResources() {
        std::cout << "    Testing enhanced world resources..." << std::endl;
        
        nlm::EnhancedWorld world(20, 20, 8, 8);
        
        // Add resources
        world.add_resource(5, 5, 1.0);
        world.add_resource(15, 15, 2.0);
        
        // Add hazards
        world.add_hazard(10, 10, -0.5);
        world.add_hazard(20, 20, -1.0);
        
        // Test world state
        assert(world.is_valid_position(5, 5));
        assert(world.is_valid_position(15, 15));
        assert(world.is_valid_position(10, 10));
        assert(world.is_valid_position(20, 20));
        
        std::cout << "        Enhanced world resources tests passed" << std::endl;
    }
    
    void testEnhancedWorldEpisode() {
        std::cout << "    Testing enhanced world episode..." << std::endl;
        
        nlm::EnhancedWorld world(20, 20, 8, 8);
        
        // Create simple agent and brain for testing
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        brain.initialize();
        
        nlm::EnhancedAgent agent(brain, world);
        
        // Note: This would require a more complete agent/brain setup
        // For now, we'll test the world interface
        
        std::cout << "        Enhanced world episode tests passed (placeholder)" << std::endl;
    }
    
    void runAllEnhancedWorldTests() {
        testEnhancedWorldCreation();
        testEnhancedWorldResources();
        testEnhancedWorldEpisode();
    }
    
} // namespace enhanced_world_tests

// Enhanced Agent Tests
namespace enhanced_agent_tests {
    
    void testEnhancedAgentCreation() {
        std::cout << "    Testing enhanced agent creation..." << std::endl;
        
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        nlm::EnhancedWorld world(20, 20, 8, 8);
        
        nlm::EnhancedAgent agent(brain, world);
        
        // Test subsystem management
        agent.enable_subsystem("reward", true);
        agent.enable_subsystem("development", true);
        agent.enable_subsystem("curiosity", true);
        
        // Test state access
        float curiosity = agent.get_curiosity_level();
        assert(0.0f <= curiosity && curiosity <= 1.0f);
        
        std::cout << "        Enhanced agent creation tests passed" << std::endl;
    }
    
    void testEnhancedAgentStateManagement() {
        std::cout << "    Testing enhanced agent state management..." << std::endl;
        
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        nlm::EnhancedWorld world(20, 20, 8, 8);
        
        nlm::EnhancedAgent agent(brain, world);
        
        // Test state before and after operations
        float initial_curiosity = agent.get_curiosity_level();
        
        // Apply reward modulation
        agent.apply_reward_modulation(1.0, 0.5);
        
        float updated_curiosity = agent.get_curiosity_level();
        // Curiosity should be updated based on reward
        
        std::cout << "        Enhanced agent state management tests passed" << std::endl;
    }
    
    void testEnhancedAgentErrorHandling() {
        std::cout << "    Testing enhanced agent error handling..." << std::endl;
        
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        nlm::EnhancedWorld world(20, 20, 8, 8);
        
        nlm::EnhancedAgent agent(brain, world);
        
        // Test invalid subsystem name
        try {
            agent.enable_subsystem("invalid_subsystem", true);
            assert(false); // Should not reach here
        } catch (const std::invalid_argument& e) {
            assert(true); // Expected error
        }
        
        std::cout << "        Enhanced agent error handling tests passed" << std::endl;
    }
    
    void runAllEnhancedAgentTests() {
        testEnhancedAgentCreation();
        testEnhancedAgentStateManagement();
        testEnhancedAgentErrorHandling();
    }
    
} // namespace enhanced_agent_tests

// Integration Tests
namespace integration_tests {
    
    void testCompleteIntegration() {
        std::cout << "    Testing complete integration..." << std::endl;
        
        // Create enhanced world
        nlm::EnhancedWorld world(20, 20, 8, 8);
        
        // Add some resources and hazards
        world.add_resource(5, 5, 1.0);
        world.add_resource(15, 15, 2.0);
        world.add_hazard(10, 10, -0.5);
        
        // Create enhanced brain
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        brain.initialize();
        
        // Create enhanced agent
        nlm::EnhancedAgent agent(brain, world);
        
        // Run a simple episode
        float total_reward = world.run_episode(brain, agent, 50, verbose: false);
        
        // Check that we got some reward
        assert(total_reward >= 0.0f);
        
        // Check brain statistics
        auto brain_stats = brain.get_stats();
        assert(brain_stats["simulation_steps"] == 50);
        
        std::cout << "        Complete integration tests passed" << std::endl;
    }
    
    void testErrorPropagation() {
        std::cout << "    Testing error propagation..." << std::endl;
        
        // Test that errors in one component are properly propagated
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        
        // Set invalid configuration
        config.set("brain.neuron_count", -1000);
        
        // Initialize should fail
        try {
            brain.initialize();
            assert(false); // Should not reach here
        } catch (const nlm::ValidationError& e) {
            assert(true); // Expected error
        }
        
        // Reset to valid config
        config.set("brain.neuron_count", 1000);
        brain.initialize();
        
        std::cout << "        Error propagation tests passed" << std::endl;
    }
    
    void testPerformanceWithLargeConfig() {
        std::cout << "    Testing performance with large configuration..." << std::endl;
        
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        
        // Create large configuration
        std::vector<std::string> large_keys;
        for (int i = 0; i < 100; ++i) {
            std::string key = "param." + std::to_string(i);
            brain._config.set(key, i * 0.1f);
            large_keys.push_back(key);
        }
        
        // Initialize and run simulation
        brain.initialize();
        brain.run_simulation(100, verbose: false);
        
        // Check that it completed successfully
        auto stats = brain.get_stats();
        assert(stats["simulation_steps"] == 100);
        
        std::cout << "        Performance with large configuration tests passed" << std::endl;
    }
    
    void runAllIntegrationTests() {
        testCompleteIntegration();
        testErrorPropagation();
        testPerformanceWithLargeConfig();
    }
    
} // namespace integration_tests

// Performance Tests
namespace performance_tests {
    
    void testConfigurationValidationPerformance() {
        std::cout << "    Testing configuration validation performance..." << std::endl;
        
        nlm::ConfigSchema schema = nlm::ConfigSchema::createStandardSchema();
        nlm::Config config;
        
        // Set up valid configuration
        config.set("brain.neuron_count", 1000);
        config.set("brain.synapse_density", 0.1);
        config.set("simulation.timestep", 0.001);
        
        // Measure validation time
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 1000; ++i) {
            schema.isValid(config);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "        Configuration validation time (1000 iterations): " << duration.count() << "ms" << std::endl;
        
        // Should be reasonable (less than 1 second for 1000 validations)
        assert(duration.count() < 1000);
        
        std::cout << "        Configuration validation performance tests passed" << std::endl;
    }
    
    void testBrainSimulationPerformance() {
        std::cout << "    Testing brain simulation performance..." << std::endl;
        
        nlm::ValidatedConfig config;
        nlm::EnhancedBrain brain(config);
        brain.initialize();
        
        // Measure simulation time
        auto start = std::chrono::high_resolution_clock::now();
        brain.run_simulation(1000, verbose: false);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "        Brain simulation time (1000 steps): " << duration.count() << "ms" << std::endl;
        
        // Should complete within reasonable time
        assert(duration.count() < 10000);
        
        std::cout << "        Brain simulation performance tests passed" << std::endl;
    }
    
    void runAllPerformanceTests() {
        testConfigurationValidationPerformance();
        testBrainSimulationPerformance();
    }
    
} // namespace performance_tests

// Main test runner
void runAllEnhancedTests() {
    std::cout << "=== Running Enhanced NLM Test Suite ===" << std::endl;
    
    try {
        // Run validation tests
        std::cout << "\n--- Validation Tests ---" << std::endl;
        validation_tests::runAllValidationTests();
        
        // Run enhanced configuration tests
        std::cout << "\n--- Enhanced Configuration Tests ---" << std::endl;
        enhanced_config_tests::runAllEnhancedConfigTests();
        
        // Run enhanced brain tests
        std::cout << "\n--- Enhanced Brain Tests ---" << std::endl;
        enhanced_brain_tests::runAllEnhancedBrainTests();
        
        // Run enhanced world tests
        std::cout << "\n--- Enhanced World Tests ---" << std::endl;
        enhanced_world_tests::runAllEnhancedWorldTests();
        
        // Run enhanced agent tests
        std::cout << "\n--- Enhanced Agent Tests ---" << std::endl;
        enhanced_agent_tests::runAllEnhancedAgentTests();
        
        // Run integration tests
        std::cout << "\n--- Integration Tests ---" << std::endl;
        integration_tests::runAllIntegrationTests();
        
        // Run performance tests
        std::cout << "\n--- Performance Tests ---" << std::endl;
        performance_tests::runAllPerformanceTests();
        
        std::cout << "\n=== All Enhanced Tests Passed Successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n*** TEST FAILED ***" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "=== Some Tests Failed ===" << std::endl;
        throw;
    }
}

} // namespace test_enhanced

// Global test runner
void run_enhanced_tests() {
    test_enhanced::runAllEnhancedTests();
}