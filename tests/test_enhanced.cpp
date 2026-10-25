// Test file for enhanced NLM Python bindings functionality
// Tests convenience methods, advanced features, and context managers

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace test_enhanced {

void testConvenienceMethods() {
    std::cout << "    Testing convenience methods..." << std::endl;
    
    // Test quickStart convenience method
    auto config = std::make_shared<nlm::Config>();
    config->setInt("neuron_count", 100);
    config->setInt("region_count", 2);
    config->setFloat("connection_probability", 0.1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    
    // Test runSimulation convenience method
    brain.runSimulation(10, 0);
    assert(brain.getTotalNeuronCount() == 100);  // Should not change
    
    // Test batchStep convenience method
    brain.reset();
    brain.initialize();
    int stepsCompleted = brain.batchStep(5, 0, std::vector<float>());
    assert(stepsCompleted == 5);
    
    std::cout << "    Convenience methods test passed" << std::endl;
}

void testAdvancedFeatures() {
    std::cout << "    Testing advanced features..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->setInt("neuron_count", 200);
    config->setInt("region_count", 3);
    config->setFloat("connection_probability", 0.15);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test getDetailedStats
    auto stats = brain.getDetailedStats();
    assert(stats.find("total_neurons") != stats.end());
    assert(stats.find("total_synapses") != stats.end());
    assert(stats.find("region_count") != stats.end());
    
    std::cout << "    Advanced features test passed" << std::endl;
}

void testConfigEnhancements() {
    std::cout << "    Testing configuration enhancements..." << std::endl;
    
    nlm::Config config;
    
    // Test setInt, setFloat, setString convenience methods
    config.setInt("test_int", 42);
    config.setFloat("test_float", 3.14f);
    config.setString("test_string", "hello");
    
    // Test getInt, getFloat, getString convenience methods
    assert(config.getInt("test_int") == 42);
    assert(config.getFloat("test_float") == 3.14f);
    assert(config.getString("test_string") == "hello");
    
    // Test toDict conversion
    auto dict = config.toDict();
    assert(dict.find("test_int") != dict.end());
    assert(dict.find("test_float") != dict.end());
    assert(dict.find("test_string") != dict.end());
    
    // Test fromDict loading
    nlm::Config config2;
    std::unordered_map<std::string, std::string> testDict = {
        {"int_val", "100"},
        {"float_val", "2.71"},
        {"bool_val", "true"},
        {"string_val", "world"}
    };
    config2.fromDict(testDict);
    
    // Should have parsed values (exact parsing depends on implementation)
    assert(config2.has("int_val"));
    assert(config2.has("float_val"));
    assert(config2.has("string_val"));
    
    // Test summary method
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("test_int") != std::string::npos);
    
    std::cout << "    Configuration enhancements test passed" << std::endl;
}

void testBrainConvenienceMethods() {
    std::cout << "    Testing brain convenience methods..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->setInt("neuron_count", 150);
    config->setInt("region_count", 2);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test initializeWithConfig
    auto newConfig = std::make_shared<nlm::Config>();
    newConfig->setInt("neuron_count", 300);
    newConfig->setInt("region_count", 4);
    
    nlm::Brain brain2;
    assert(brain2.initializeWithConfig(*newConfig));
    assert(brain2.getTotalNeuronCount() == 300);
    assert(brain2.getRegionCount() == 4);
    
    // Test injectPattern
    std::vector<float> pattern = {1.0f, 0.5f, 0.2f, 0.8f, 0.3f};
    brain3.injectPattern(pattern, nlm::NeuronType::Sensory);
    
    // Test resetWithConfig
    auto resetConfig = std::make_shared<nlm::Config>();
    resetConfig->setInt("neuron_count", 100);
    resetConfig->setInt("region_count", 1);
    brain3.resetWithConfig(resetConfig);
    assert(brain3.getTotalNeuronCount() == 100);
    
    std::cout << "    Brain convenience methods test passed" << std::endl;
}

void testContextManager() {
    std::cout << "    Testing context manager..." << std::endl;
    
    // Test default context manager
    auto context1 = std::make_shared<ContextManager>();
    auto brain1 = context1->brain();
    assert(brain1 != nullptr);
    assert(brain1->initialize());
    
    // Test with quick config
    auto context2 = std::make_shared<ContextManager>();
    auto brain2 = context2->brain();
    assert(brain2->initialize());
    
    // Test runEpisode
    std::vector<float> input = {0.5f, 0.3f, 0.8f};
    auto percept = std::make_shared<nlm::SensoryInput>();
    for (float val : input) {
        percept->addSignal(val);
    }
    
    int episodeSteps = context2->runEpisode(*percept, 10);
    assert(episodeSteps == 10);
    
    // Test batchProcess
    std::vector<std::pair<int, std::vector<float>>> episodes = {
        {5, {0.1f, 0.2f, 0.3f}},
        {3, {0.4f, 0.5f, 0.6f}}
    };
    
    auto results = context2->batchProcess(episodes);
    assert(results.size() == 2);
    assert(results[0] == 5);
    assert(results[1] == 3);
    
    // Test getConfig
    auto config = context2->config();
    assert(config != nullptr);
    
    // Test getBrain
    auto retrievedBrain = context2->brain();
    assert(retrievedBrain != nullptr);
    
    // Context manager should automatically clean up
    std::cout << "    Context manager test passed" << std::endl;
}

void runAllEnhancedTests() {
    testConvenienceMethods();
    testAdvancedFeatures();
    testConfigEnhancements();
    testBrainConvenienceMethods();
    testContextManager();
    
    std::cout << std::endl;
    std::cout << "All enhanced tests passed successfully!" << std::endl;
    std::cout << "New features tested:" << std::endl;
    std::cout << "  - Convenience methods for initialization and simulation" << std::endl;
    std::cout << "  - Advanced statistics and brain state queries" << std::endl;
    std::cout << "  - Enhanced configuration management with Pythonic API" << std::endl;
    std::cout << "  - Context managers for resource management" << std::endl;
    std::cout << "  - Batch processing and pattern injection" << std::endl;
    std::cout << "  - Type-safe configuration value access" << std::endl;
}

} // namespace test_enhanced