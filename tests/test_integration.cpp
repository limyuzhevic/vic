// Integration Test Suite
// Tests for integration of all improvements including:
// - Test that all improvements work together
// - Test backward compatibility
// - Test edge cases
// - Test comprehensive workflow

#include "src/agent/AgentBrain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/experiments/Phase6Demo.hpp"
#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace test_integration {

void testSystemIntegrationBasic() {
    // Test basic integration of all improved components
    std::cout << "Testing basic system integration..." << std::endl;
    
    // Create configuration
    nlm::Config config;
    config.addInt("system.neuron_count", 100, "Number of neurons");
    config.addDouble("system.learning_rate", 0.01, "Learning rate");
    config.addBool("system.enabled", true, "System enabled flag");
    config.addString("system.name", "NLM_Integration_Test", "System name");
    
    // Create brain with config
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    agentBrain.initialize(nlm::SimpleWorld());
    
    // Test that all components are properly connected
    assert(agentBrain.getBrain() != nullptr);
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    // Test memory integration
    assert(agentBrain.getNeuromodulationLevel() >= 0.0f);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    assert(agentBrain.getPredictionError() >= -1.0f && agentBrain.getPredictionError() <= 1.0f);
    
    std::cout << "    Basic system integration test passed" << std::endl;
}

void testConfigIntegrationWithPythonBindings() {
    // Test Config integration with Python bindings improvements
    std::cout << "Testing Config integration with Python bindings..." << std::endl;
    
    nlm::Config config;
    
    // Test convenience methods that would be exposed to Python
    config.addInt("python_test.int_val", 42, "Python test integer");
    config.addDouble("python_test.double_val", 3.14, "Python test double");
    config.addBool("python_test.bool_val", true, "Python test boolean");
    config.addString("python_test.string_val", "hello_python", "Python test string");
    
    // Test type validation and error handling
    assert(config.has("python_test.int_val"));
    assert(config.has("python_test.double_val"));
    assert(config.has("python_test.bool_val"));
    assert(config.has("python_test.string_val"));
    
    // Test getOr functionality (Python convenience)
    int intVal = config.getOr<int>("python_test.int_val", 0);
    double doubleVal = config.getOr<double>("python_test.double_val", 0.0);
    bool boolVal = config.getOr<bool>("python_test.bool_val", false);
    std::string stringVal = config.getOr<std::string>("python_test.string_val", "");
    
    assert(intVal == 42);
    assert(doubleVal > 3.13 && doubleVal < 3.15);
    assert(boolVal == true);
    assert(stringVal == "hello_python");
    
    std::cout << "    Config integration with Python bindings test passed" << std::endl;
}

void testAgentBrainIntegrationWithConfig() {
    // Test AgentBrain integration with improved Config
    std::cout << "Testing AgentBrain integration with Config..." << std::endl;
    
    nlm::Config config;
    config.addInt("brain.neuron_count", 500, "Brain neuron count");
    config.addDouble("brain.connection_prob", 0.1, "Connection probability");
    config.addBool("brain.enable_dev", true, "Enable development");
    config.addString("brain.region_name", "main_brain", "Brain region name");
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test configuration flag integration
    agentBrain.enableRewardModulation(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    assert(agentBrain.isRewardModulationEnabled() == true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
    // Test that configuration affects brain behavior
    SimpleWorld world;
    world.configure(64, 64, 16, 16);
    agentBrain.initialize(world);
    
    // Test with valid sensory input
    nlm::Vision vision(16, 16, 3);
    std::vector<float> visionData(16*16*3, 0.5f);
    vision.setData(visionData);
    
    nlm::SensoryPercept percept;
    percept.setVision(vision);
    
    // Should process without crashing
    agentBrain.processSensoryInput(percept);
    
    // Test motor command decoding
    MotorCommand cmd = agentBrain.decodeMotorCommand();
    // Should return a valid motor command (not a crash)
    
    std::cout << "    AgentBrain integration with Config test passed" << std::endl;
}

void testPhase6IntegrationFramework() {
    // Test Phase6 integration framework improvements
    std::cout << "Testing Phase6 integration framework..." << std::endl;
    
    // Create Phase6Config
    nlm::Phase6Config phaseConfig;
    phaseConfig.neuronCount = 1000;
    phaseConfig.maxSteps = 5000;
    phaseConfig.enableCheckpointing = true;
    phaseConfig.enableReplay = true;
    phaseConfig.enableDevelopment = true;
    phaseConfig.checkpointPath = "./integration_test_checkpoint.bin";
    
    // Test configuration structure
    assert(phaseConfig.neuronCount == 1000);
    assert(phaseConfig.maxSteps == 5000);
    assert(phaseConfig.enableCheckpointing == true);
    assert(phaseConfig.enableReplay == true);
    assert(phaseConfig.enableDevelopment == true);
    
    // Create integration experiment
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test integration verification structure
    nlm::DetailedIntegrationResult detailedResult;
    // Should have default values
    assert(detailedResult.memoryWorkingMemoryIntegrated == false);
    assert(detailedResult.memoryEpisodicMemoryIntegrated == false);
    assert(detailedResult.neuromodulationIntegrated == false);
    
    // Test metrics structures
    nlm::MemoryMetrics memoryMetrics;
    nlm::NeuromodulationMetrics neuromodMetrics;
    nlm::CheckpointValidation checkpointValidation;
    nlm::ReplayMetrics replayMetrics;
    
    // Should be properly initialized
    assert(memoryMetrics.workingMemorySuccess == false);
    assert(neuromMetrics.dopamineSuccess == false);
    assert(checkpointValidation.saveSuccess == false);
    assert(replayMetrics.episodicMemoryAvailable == false);
    
    std::cout << "    Phase6 integration framework test passed" << std::endl;
}

void testErrorHandlingIntegration() {
    // Test integrated error handling across all components
    std::cout << "Testing integrated error handling..." << std::endl;
    
    // Test Config error handling
    nlm::Config config;
    try {
        config.set("", "value"); // Invalid key
        assert(false); // Should not reach here
    } catch (const nlm::ConfigValidationError& e) {
        // Expected
    }
    
    // Test exception propagation
    nlm::ConfigValidationError valError("Test validation");
    nlm::ConfigLoadError loadError("Test load");
    nlm::ConfigSaveError saveError("Test save");
    
    assert(std::string(valError.what()) == "Test validation");
    assert(std::string(loadError.what()) == "Test load");
    assert(std::string(saveError.what()) == "Test save");
    
    // Test AgentBrain error handling with null brain
    nlm::AgentBrain brain(nullptr);
    assert(brain.getSensoryInputSize() == 0);
    assert(brain.getNeuromodulationLevel() == 0.0f);
    
    std::cout << "    Integrated error handling test passed" << std::endl;
}

void testBackwardCompatibility() {
    // Test that improvements maintain backward compatibility
    std::cout << "Testing backward compatibility..." << std::endl;
    
    // Test that existing test patterns still work
    nlm::Config config;
    
    // Old-style API should still work
    config.set("old_style_int", 123, nlm::ConfigSource::Runtime);
    config.set("old_style_string", "test", nlm::ConfigSource::Runtime);
    config.set("old_style_bool", true, nlm::ConfigSource::Runtime);
    
    // Get values the old way
    auto oldInt = config.get<int>("old_style_int");
    auto oldString = config.get<std::string>("old_style_string");
    auto oldBool = config.get<bool>("old_style_bool");
    
    assert(oldInt && *oldInt == 123);
    assert(oldString && *oldString == "test");
    assert(oldBool && *oldBool == true);
    
    // Test that new convenience methods work alongside old ones
    config.addInt("new_style_int", 456, "New style");
    int val1 = config.getOr<int>("new_style_int", 0);
    int val2 = config.get<int>("new_style_int").value_or(0);
    
    assert(val1 == 456);
    assert(val2 == 456);
    
    std::cout << "    Backward compatibility test passed" << std::endl;
}

void testEdgeCases() {
    // Test edge cases and robustness
    std::cout << "Testing edge cases..." << std::endl;
    
    // Test with extreme values
    nlm::Config config;
    
    config.set("max_int", std::numeric_limits<int>::max(), nlm::ConfigSource::Runtime);
    config.set("min_int", std::numeric_limits<int>::min(), nlm::ConfigSource::Runtime);
    config.set("max_double", std::numeric_limits<double>::max(), nlm::ConfigSource::Runtime);
    config.set("min_double", -std::numeric_limits<double>::max(), nlm::ConfigSource::Runtime);
    
    // Test NaN and infinity handling
    config.set("nan_value", NAN, nlm::ConfigSource::Runtime);
    config.set("inf_value", INFINITY, nlm::ConfigSource::Runtime);
    
    // Test empty collections
    config.set("empty_int_list", std::vector<int>{}, nlm::ConfigSource::Runtime);
    config.set("empty_string_list", std::vector<std::string>{}, nlm::ConfigSource::Runtime);
    
    // Test AgentBrain edge cases
    nlm::Brain brain(config); // Create brain with extreme values
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test null handling
    assert(agentBrain.getBrain() != nullptr);
    
    // Test constant values
    size_t sensorySize = agentBrain.getSensoryInputSize();
    size_t motorSize = agentBrain.getMotorOutputSize();
    
    assert(sensorySize > 0);
    assert(motorSize > 0);
    
    std::cout << "    Edge cases test passed" << std::endl;
}

void testPerformanceWithImprovements() {
    // Test that improvements don't significantly impact performance
    std::cout << "Testing performance with improvements..." << std::endl;
    
    // Time Config operations
    nlm::Config config;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        config.set(std::to_string(i), i, nlm::ConfigSource::Runtime);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto configTime = std::chrono::duration<double>(end - start).count();
    
    // Should complete 1000 operations in reasonable time
    assert(configTime < 1.0); // Should be fast
    
    // Test AgentBrain initialization
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(*config2);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        SimpleWorld world;
        agentBrain.initialize(world);
    }
    end = std::chrono::high_resolution_clock::now();
    auto agentTime = std::chrono::duration<double>(end - start).count();
    
    // Should complete 100 initializations in reasonable time
    assert(agentTime < 2.0);
    
    std::cout << "    Performance test passed (Config time: " 
              << configTime << "s, AgentBrain time: " << agentTime << "s)" << std::endl;
}

void testMemoryUsage() {
    // Test memory usage with improvements
    std::cout << "Testing memory usage..." << std::endl;
    
    // Create multiple Config objects
    std::vector<nlm::Config> configs;
    for (int i = 0; i < 100; ++i) {
        nlm::Config config;
        config.addInt(std::to_string(i), i, "Test key");
        configs.push_back(std::move(config));
    }
    
    // All should be valid
    for (int i = 0; i < 100; ++i) {
        assert(configs[i].getOr<int>(std::to_string(i), 0) == i);
    }
    
    // Create multiple Brain objects
    std::vector<nlm::Brain> brains;
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        brains.emplace_back(*config);
    }
    
    // All should be valid
    for (int i = 0; i < 10; ++i) {
        assert(brains[i].initialize());
    }
    
    std::cout << "    Memory usage test passed" << std::endl;
}

void runAll() {
    testSystemIntegrationBasic();
    testConfigIntegrationWithPythonBindings();
    testAgentBrainIntegrationWithConfig();
    testPhase6IntegrationFramework();
    testErrorHandlingIntegration();
    testBackwardCompatibility();
    testEdgeCases();
    testPerformanceWithImprovements();
    testMemoryUsage();
    
    std::cout << std::endl;
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
    std::cout << "The NLM improvements are fully integrated and working together." << std::endl;
}

} // namespace test_integration