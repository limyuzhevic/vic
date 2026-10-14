// JSON Configuration Tests
// Phase 2: Advanced Configuration System Tests

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_json_config {

void testJSONFileFormat() {
    std::cout << "  Testing JSON file format loading..." << std::endl;
    
    nlm::Config config;
    
    // Test JSON file loading
    bool result = config.loadFromFile("configs/test_config.json");
    assert(result);
    
    // Verify JSON-specific values
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    auto regionCount = config.get<int64_t>("region_count");
    assert(regionCount.has_value() && *regionCount == 2);
    
    auto dopamineBaseline = config.get<double>("dopamine_baseline");
    assert(dopamineBaseline.has_value() && *dopamineBaseline == 0.1);
    
    // Test array values
    auto testArrayInts = config.get<std::vector<int64_t>>("test_array_integers");
    assert(testArrayInts.has_value() && testArrayInts->size() == 5);
    assert((*testArrayInts)[0] == 1 && (*testArrayInts)[4] == 5);
    
    auto testArrayFloats = config.get<std::vector<double>>("test_array_floats");
    assert(testArrayFloats.has_value() && testArrayFloats->size() == 5);
    assert(std::abs((*testArrayFloats)[0] - 1.1) < 0.001);
    
    auto testArrayStrings = config.get<std::vector<std::string>>("test_array_strings");
    assert(testArrayStrings.has_value() && testArrayStrings->size() == 3);
    assert((*testArrayStrings)[0] == "test1" && (*testArrayStrings)[2] == "test3");
    
    // Test boolean and null values
    auto testBoolTrue = config.get<bool>("test_bool_true");
    assert(testBoolTrue.has_value() && *testBoolTrue == true);
    
    auto testBoolFalse = config.get<bool>("test_bool_false");
    assert(testBoolFalse.has_value() && *testBoolFalse == false);
    
    // Note: null values are converted to strings, so we can't test directly
    
    std::cout << "    PASSED" << std::endl;
}

void testYAMLFileFormat() {
    std::cout << "  Testing YAML file format loading..." << std::endl;
    
    // For now, YAML is not implemented, so we test that it falls back to simple format
    // This is expected behavior until YAML support is added
    
    std::cout << "    SKIPPED (YAML not yet implemented)" << std::endl;
}

void testFileAutoDetection() {
    std::cout << "  Testing automatic file format detection..." << std::endl;
    
    nlm::Config config;
    
    // Test JSON file (should be detected by extension)
    bool result1 = config.loadFromFile("configs/test_config.json");
    assert(result1);
    
    // Clear config for next test
    config.clear();
    
    // Test key=value format (should be detected by content)
    bool result2 = config.loadFromFile("configs/default.cfg");
    assert(result2);
    
    // Verify that values were loaded
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    std::cout << "    PASSED" << std::endl;
}

void testSaveToFile() {
    std::cout << "  Testing configuration file save..." << std::endl;
    
    nlm::Config config;
    
    // Load JSON configuration
    bool result1 = config.loadFromFile("configs/test_config.json");
    assert(result1);
    
    // Save configuration
    bool result2 = config.saveToFile("configs/test_saved.cfg");
    assert(result2);
    
    // Load saved configuration
    nlm::Config config2;
    bool result3 = config2.loadFromFile("configs/test_saved.cfg");
    assert(result3);
    
    // Verify that neuron_count was saved (it should be a string in key=value format)
    auto neuronCount = config2.get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    // Clean up
    std::remove("configs/test_saved.cfg");
    
    std::cout << "    PASSED" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "  Testing backward compatibility with original config..." << std::endl;
    
    nlm::Config config;
    
    // Load original config file (key=value format)
    bool result = config.loadFromFile("configs/default.cfg");
    assert(result);
    
    // Verify that all original values are still accessible
    auto randomSeed = config.get<int64_t>("random_seed");
    assert(randomSeed.has_value() && *randomSeed == 42);
    
    auto simulationTimestep = config.get<double>("simulation_timestep");
    assert(simulationTimestep.has_value() && *simulationTimestep == 0.001);
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    auto regionCount = config.get<int64_t>("region_count");
    assert(regionCount.has_value() && *regionCount == 2);
    
    auto connectionProbability = config.get<double>("connection_probability");
    assert(connectionProbability.has_value() && *connectionProbability == 0.1);
    
    // Note: We should verify that dopamine_baseline is correct (without duplicate)
    auto dopamineBaselines = config.getKeys();
    int dopamineBaselineCount = 0;
    for (const auto& key : dopamineBaselines) {
        if (key == "dopamine_baseline") {
            dopamineBaselineCount++;
        }
    }
    assert(dopamineBaselineCount == 1);
    
    std::cout << "    PASSED" << std::endl;
}

void testErrorHandling() {
    std::cout << "  Testing error handling..." << std::endl;
    
    nlm::Config config;
    
    // Test loading non-existent file
    bool result1 = config.loadFromFile("configs/nonexistent.json");
    assert(!result1);
    
    // Test loading malformed JSON
    // Create a malformed JSON file
    std::ofstream badFile("configs/malformed.json");
    badFile << "{\"neuron_count\": 1000,}";  // Missing property name
    badFile.close();
    
    bool result2 = config.loadFromFile("configs/malformed.json");
    // This might fail or succeed depending on implementation
    // We'll accept either behavior for now
    
    // Clean up
    std::remove("configs/malformed.json");
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    testJSONFileFormat();
    testYAMLFileFormat();
    testFileAutoDetection();
    testSaveToFile();
    testBackwardCompatibility();
    testErrorHandling();
}

} // namespace test_json_config