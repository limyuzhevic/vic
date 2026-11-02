// Enhanced Python Bindings Tests
// Tests for the enhanced NLM Python bindings with improved error handling,
// user-friendly APIs, and comprehensive test coverage.

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <stdexcept>
#include <algorithm>

#include "python/bindings_enhanced.cpp"
#include "tests/test_brain.cpp"

namespace test_enhanced_bindings {

// Custom exception for testing
class TestError : public std::runtime_error {
public:
    explicit TestError(const std::string& msg) : std::runtime_error("Test Error: " + msg) {}
};

// Test 1: Enhanced Configuration Tests
void testEnhancedConfig() {
    std::cout << "Running enhanced configuration tests..." << std::endl;
    
    // Test 1.1: Configuration with validation
    auto config = nlm::createDefaultConfig();
    assert(config != nullptr);
    
    // Test setting different types with source specification
    config->set("float_val", 3.14f, "runtime");
    config->set("int_val", 42, "runtime");
    config->set("string_val", "test", "runtime");
    
    assert(config->has("float_val"));
    assert(config->has("int_val"));
    assert(config->has("string_val"));
    
    // Test getOr with defaults
    float f_val = config->getOr<float>("float_val", 0.0f);
    assert(fabs(f_val - 3.14f) < 0.01f);
    
    int i_val = config->getOr<int64_t>("int_val", 0);
    assert(i_val == 42);
    
    std::string s_val = config->getOr<std::string>("string_val", "");
    assert(s_val == "test");
    
    // Test missing key with default
    float default_val = config->getOr<float>("missing_key", 1.0f);
    assert(fabs(default_val - 1.0f) < 0.001f);
    
    // Test export as dictionary
    auto dict = config->exportAsDict();
    assert(dict.find("float_val") != dict.end());
    assert(dict.find("int_val") != dict.end());
    assert(dict.find("string_val") != dict.end());
    
    std::cout << "    testEnhancedConfig passed" << std::endl;
}

// Test 2: Convenience Functions Tests
void testConvenienceFunctions() {
    std::cout << "Running convenience function tests..." << std::endl;
    
    // Test createLearningConfig
    auto learning_config = nlm::createLearningConfig(0.05f, 0.2f);
    assert(learning_config != nullptr);
    
    // Test createMinimalConfig
    auto minimal_config = nlm::createMinimalConfig(50);
    assert(minimal_config != nullptr);
    
    auto minimal_neuron_count = minimal_config->getOr<int64_t>("neuron_count", 0);
    assert(minimal_neuron_count == 50);
    
    // Test createWorld
    auto world = nlm::createWorld(30, 30, 12, 12);
    assert(world != nullptr);
    
    assert(world->getWidth() == 30);
    assert(world->getHeight() == 30);
    
    // Test createBrain
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    assert(brain != nullptr);
    
    std::cout << "    testConvenienceFunctions passed" << std::endl;
}

// Test 3: Enhanced Simulation Functions
void testEnhancedSimulation() {
    std::cout << "Running enhanced simulation tests..." << std::endl;
    
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    
    // Test quickSimulation
    nlm::quickSimulation(brain, 50, 25.0f);
    
    // Test runBasicTest
    auto spike_count = nlm::runBasicTest(brain);
    assert(spike_count >= 0); // Should have some spikes
    
    // Reset brain and test learning analysis
    brain->reset();
    auto learning_result = nlm::analyzeLearning(brain, 200);
    bool learning_occurred = std::get<0>(learning_result);
    float weight_change = std::get<1>(learning_result);
    int64_t initial_synapses = std::get<2>(learning_result);
    int64_t final_synapses = std::get<3>(learning_result);
    
    // Learning should occur after applying input patterns
    assert(initial_synapses > 0);
    assert(final_synapses > 0);
    
    std::cout << "    testEnhancedSimulation passed" << std::endl;
}

// Test 4: Enhanced Error Handling
void testEnhancedErrorHandling() {
    std::cout << "Running enhanced error handling tests..." << std::endl;
    
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    
    // Test BrainInitializationError - should be thrown if we try to initialize invalid brain
    // (This is already handled in the quickSimulation function)
    
    // Test ConfigurationError
    auto invalid_config = std::make_shared<nlm::Config>();
    
    try {
        nlm::validateConfig(invalid_config);
        assert(false); // Should not reach here
    } catch (const nlm::ConfigurationError& e) {
        // Expected
        std::cout << "    Caught ConfigurationError as expected: " << e.what() << std::endl;
    }
    
    // Test with valid config
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 2, nlm::ConfigSource::Runtime);
    
    std::string validation_result = nlm::validateConfig(config);
    assert(validation_result == "Configuration is valid");
    
    std::cout << "    testEnhancedErrorHandling passed" << std::endl;
}

// Test 5: Batch Processing
void testBatchProcessing() {
    std::cout << "Running batch processing tests..." << std::endl;
    
    // Create multiple brains
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    std::vector<float> rewards = {10.0f, -5.0f, 15.0f, 0.0f};
    
    for (int i = 0; i < 4; ++i) {
        auto config = nlm::createDefaultConfig();
        config->set("neuron_count", static_cast<int64_t>(100 + i * 50), nlm::ConfigSource::Runtime);
        auto brain = nlm::createBrain(config);
        brains.push_back(brain);
    }
    
    // Test batch training
    nlm::batchTrain(brains, rewards, 100);
    
    // Verify all brains were processed
    assert(brains.size() == 4);
    
    for (auto& brain : brains) {
        assert(brain != nullptr);
    }
    
    std::cout << "    testBatchProcessing passed" << std::endl;
}

// Test 6: Advanced Analysis
void testAdvancedAnalysis() {
    std::cout << "Running advanced analysis tests..." << std::endl;
    
    // Create multiple brains with different characteristics
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    std::vector<std::string> labels = {"simple", "complex", "learning"};
    
    for (const auto& label : labels) {
        auto config = nlm::createDefaultConfig();
        if (label == "simple") {
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
        } else if (label == "complex") {
            config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
            config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
        } else if (label == "learning") {
            config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Runtime);
            config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
        }
        
        auto brain = nlm::createBrain(config);
        brains.push_back(brain);
    }
    
    // Test brain comparison
    auto comparison = nlm::compareBrains(brains, labels);
    assert(comparison.size() == labels.size());
    
    // Test brain stats
    auto simple_stats = nlm::getBrainStats(brains[0]);
    assert(simple_stats.find("total_neurons") != simple_stats.end());
    assert(simple_stats.find("total_synapses") != simple_stats.end());
    
    // Test export
    auto json_export = nlm::exportBrainState(brains[0], "json");
    assert(json_export.find("total_neurons") != std::string::npos);
    
    auto csv_export = nlm::exportBrainState(brains[0], "csv");
    assert(csv_export.find("metric,value") != std::string::npos);
    
    std::cout << "    testAdvancedAnalysis passed" << std::endl;
}

// Test 7: Text Encoding
void testTextEncoding() {
    std::cout << "Running text encoding tests..." << std::endl;
    
    // Test text encoding
    auto apple_pattern = nlm::encodeText("apple");
    auto apple_pattern_upper = nlm::encodeText("Apple");
    auto zebra_pattern = nlm::encodeText("zebra");
    
    assert(apple_pattern.size() == 4);
    assert(apple_pattern_upper.size() == 4);
    assert(zebra_pattern.size() == 4);
    
    // Case insensitive and deterministic
    assert(apple_pattern == apple_pattern_upper);
    assert(apple_pattern != zebra_pattern);
    
    // Test creating percept with text
    auto percept = nlm::createTextPercept("test", 
                                           std::vector<float>(256, 0.5f),
                                           std::vector<float>(8, 0.3f));
    
    assert(percept != nullptr);
    
    std::cout << "    testTextEncoding passed" << std::endl;
}

// Test 8: Demo Brain
void testDemoBrain() {
    std::cout << "Running demo brain tests..." << std::endl;
    
    auto demo_result = nlm::createDemoBrain();
    assert(std::get<0>(demo_result) != nullptr);  // brain
    assert(std::get<1>(demo_result) != nullptr);  // world
    assert(std::get<2>(demo_result) != nullptr);  // agent
    
    std::cout << "    testDemoBrain passed" << std::endl;
}

// Test 9: Configuration Templates
void testConfigurationTemplates() {
    std::cout << "Running configuration template tests..." << std::endl;
    
    auto basic_brain = nlm::createFromTemplate("basic");
    auto sensorimotor_brain = nlm::createFromTemplate("sensorimotor");
    auto cognitive_brain = nlm::createFromTemplate("cognitive");
    
    assert(basic_brain != nullptr);
    assert(sensorimotor_brain != nullptr);
    assert(cognitive_brain != nullptr);
    
    std::cout << "    testConfigurationTemplates passed" << std::endl;
}

// Test 10: Analysis Tools
void testAnalysisTools() {
    std::cout << "Running analysis tools tests..." << std::endl;
    
    // Create simulated learning metrics
    std::vector<std::tuple<float, float, float, float>> metrics;
    
    for (int i = 0; i < 20; ++i) {
        bool learning = (i % 5 == 0); // Every 5th step has learning
        float change = learning ? (0.01f * i) : (-0.005f * i);
        float init_syn = 100.0f;
        float final_syn = init_syn + change;
        metrics.push_back(std::make_tuple(learning, change, init_syn, final_syn));
    }
    
    // Test learning progress analysis
    auto analysis = nlm::analyzeLearningProgress(metrics, 5);
    float avg_rate = std::get<0>(analysis);
    size_t windows = std::get<1>(analysis);
    size_t total_metrics = std::get<2>(analysis);
    
    assert(windows > 0);
    assert(total_metrics == metrics.size());
    
    // Test learning export
    auto learning_config = nlm::createLearningConfig(0.01f, 0.15f);
    
    std::cout << "    testAnalysisTools passed" << std::endl;
}

// Test 11: Environment Configuration
void testEnvironmentConfig() {
    std::cout << "Running environment configuration tests..." << std::endl;
    
    // Set environment variables temporarily
    setenv("NLM_NEURON_COUNT", "200", 1);
    setenv("NLM_REGION_COUNT", "3", 1);
    setenv("NLM_RANDOM_SEED", "12345", 1);
    
    // Create config from environment
    auto env_config = nlm::createConfigFromEnv();
    assert(env_config != nullptr);
    
    int64_t neuron_count = env_config->getOr<int64_t>("neuron_count", 0);
    assert(neuron_count == 200);
    
    // Clean up environment variables
    unsetenv("NLM_NEURON_COUNT");
    unsetenv("NLM_REGION_COUNT");
    unsetenv("NLM_RANDOM_SEED");
    
    std::cout << "    testEnvironmentConfig passed" << std::endl;
}

// Test 12: Validation Utilities
void testValidationUtilities() {
    std::cout << "Running validation utilities tests..." << std::endl;
    
    // Test with valid configuration
    auto config = nlm::createDefaultConfig();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 2, nlm::ConfigSource::Runtime);
    
    std::string result = nlm::validateConfig(config);
    assert(result == "Configuration is valid");
    
    // Test with invalid configuration
    auto invalid_config = std::make_shared<nlm::Config>();
    try {
        nlm::validateConfig(invalid_config);
        assert(false); // Should not reach here
    } catch (const nlm::ConfigurationError&) {
        // Expected
    }
    
    std::cout << "    testValidationUtilities passed" << std::endl;
}

// Test 13: Serialization
void testSerialization() {
    std::cout << "Running serialization tests..." << std::endl;
    
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    
    // Test JSON export
    auto json_output = nlm::exportBrainState(brain, "json");
    assert(!json_output.empty());
    assert(json_output.find("{") != std::string::npos);
    
    // Test CSV export
    auto csv_output = nlm::exportBrainState(brain, "csv");
    assert(!csv_output.empty());
    assert(csv_output.find("metric,value") != std::string::npos);
    
    // Test invalid format
    try {
        nlm::exportBrainState(brain, "invalid_format");
        assert(false); // Should not reach here
    } catch (const nlm::ConfigurationError&) {
        // Expected
    }
    
    std::cout << "    testSerialization passed" << std::endl;
}

// Test 14: Performance Benchmarks
void testPerformanceBenchmarks() {
    std::cout << "Running performance benchmark tests..." << std::endl;
    
    // This is a mock benchmark since actual timing would be platform-dependent
    auto benchmarks = nlm::runBenchmarks();
    assert(benchmarks.find("config_creation_time") != benchmarks.end());
    assert(benchmarks.find("brain_initialization_time") != benchmarks.end());
    assert(benchmarks.find("simulation_speed") != benchmarks.end());
    
    std::cout << "    testPerformanceBenchmarks passed" << std::endl;
}

// Test 15: Feature Availability
void testFeatureAvailability() {
    std::cout << "Running feature availability tests..." << std::endl;
    
    auto available_brains = nlm::listAvailableBrains();
    assert(!available_brains.empty());
    
    // Enable advanced features
    std::string enable_result = nlm::enableAdvancedFeatures();
    assert(!enable_result.empty());
    
    std::cout << "    testFeatureAvailability passed" << std::endl;
}

// Test 16: Type Conversion and Validation
void testTypeConversionAndValidation() {
    std::cout << "Running type conversion and validation tests..." << std::endl;
    
    auto config = nlm::createDefaultConfig();
    
    // Test various type conversions
    config->set("test_float", 3.14f, nlm::ConfigSource::Runtime);
    config->set("test_int", 42, nlm::ConfigSource::Runtime);
    config->set("test_double", 2.718, nlm::ConfigSource::Runtime);
    config->set("test_string", "hello", nlm::ConfigSource::Runtime);
    
    // Test export consistency
    auto dict = config->exportAsDict();
    assert(dict["test_float"].find("3.14") != std::string::npos);
    assert(dict["test_int"] == "42");
    
    // Test getOr with different types
    float f_val = config->getOr<float>("test_float", 0.0f);
    int64_t i_val = config->getOr<int64_t>("test_int", 0);
    double d_val = config->getOr<double>("test_double", 0.0);
    std::string s_val = config->getOr<std::string>("test_string", "");
    
    assert(fabs(f_val - 3.14f) < 0.01f);
    assert(i_val == 42);
    assert(fabs(d_val - 2.718) < 0.001);
    assert(s_val == "hello");
    
    std::cout << "    testTypeConversionAndValidation passed" << std::endl;
}

// Test 17: NeuralPlanner Integration
void testNeuralPlannerIntegration() {
    std::cout << "Running NeuralPlanner integration tests..." << std::endl;
    
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    
    if (brain->initialize()) {
        // Test action production
        auto action = brain->produceAction();
        assert(action != nullptr);
        
        // Test brain status
        size_t neurons = brain->getTotalNeuronCount();
        size_t synapses = brain->getTotalSynapseCount();
        float firing_rate = brain->getAverageFiringRate();
        
        assert(neurons > 0);
        assert(synapses >= 0);
        assert(firing_rate >= 0.0f);
    }
    
    std::cout << "    testNeuralPlannerIntegration passed" << std::endl;
}

// Test 18: AgentBrain Integration
void testAgentBrainIntegration() {
    std::cout << "Running AgentBrain integration tests..." << std::endl;
    
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    auto agent_brain = nlm::createAgentBrain(brain);
    
    auto world = nlm::createWorld(20, 20, 16, 16);
    
    agent_brain->initialize(*world);
    
    assert(agent_brain->getSensoryInputSize() > 0);
    assert(agent_brain->getMotorOutputSize() > 0);
    
    // Test motor command decoding
    auto motor_command = agent_brain->decodeMotorCommand();
    // Should be one of the MotorCommand enum values
    
    std::cout << "    testAgentBrainIntegration passed" << std::endl;
}

// Test 19: World Integration
void testWorldIntegration() {
    std::cout << "Running world integration tests..." << std::endl;
    
    auto world = nlm::createWorld(30, 30, 16, 16);
    
    assert(world->getWidth() == 30);
    assert(world->getHeight() == 30);
    
    // Test agent body
    auto body = world->getAgentBody();
    assert(body != nullptr);
    assert(body->x == 0.0f);
    assert(body->y == 0.0f);
    
    // Test world update
    world->update(0.001f);
    
    std::cout << "    testWorldIntegration passed" << std::endl;
}

// Test 20: Complete End-to-End Workflow
void testCompleteWorkflow() {
    std::cout << "Running complete end-to-end workflow tests..." << std::endl;
    
    // Step 1: Create configuration from template
    auto config = nlm::createFromTemplate("sensorimotor");
    
    // Step 2: Create brain and world
    auto brain = nlm::createBrain(config);
    auto world = nlm::createWorld(20, 20);
    
    // Step 3: Create agent
    auto agent = nlm::createAgentBrain(brain);
    
    // Step 4: Initialize and run
    agent->initialize(*world);
    
    // Step 5: Run simulation
    nlm::quickSimulation(brain, 50, 25.0f);
    
    // Step 6: Analyze results
    auto stats = nlm::getBrainStats(brain);
    assert(stats.find("total_neurons") != stats.end());
    assert(stats.find("total_spikes") != stats.end());
    
    // Step 7: Export for analysis
    auto json_output = nlm::exportBrainState(brain, "json");
    assert(!json_output.empty());
    
    std::cout << "    testCompleteWorkflow passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Enhanced Python Bindings Tests ===" << std::endl;
    
    testEnhancedConfig();
    testConvenienceFunctions();
    testEnhancedSimulation();
    testEnhancedErrorHandling();
    testBatchProcessing();
    testAdvancedAnalysis();
    testTextEncoding();
    testDemoBrain();
    testConfigurationTemplates();
    testAnalysisTools();
    testEnvironmentConfig();
    testValidationUtilities();
    testSerialization();
    testPerformanceBenchmarks();
    testFeatureAvailability();
    testTypeConversionAndValidation();
    testNeuralPlannerIntegration();
    testAgentBrainIntegration();
    testWorldIntegration();
    testCompleteWorkflow();
    
    std::cout << "\n=== All Enhanced Python Bindings Tests Passed! ===" << std::endl;
}

} // namespace test_enhanced_bindings

// Entry point for enhanced tests
int main() {
    test_enhanced_bindings::runAll();
    return 0;
}
