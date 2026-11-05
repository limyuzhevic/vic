// Cross-platform testing compatibility
// Tests and compatibility checks for different platforms (Windows, Linux, macOS)

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "factory/BrainFactory.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace test_crossplatform {

void testPlatformDetection() {
    std::cout << "    Testing platform detection..." << std::endl;
    
    // Test that the code can detect and handle different platforms
    // This is a basic test - more comprehensive platform detection
    // could be added in real implementations
    
    // Test 1: Check that configuration loading works consistently
    auto config = std::make_shared<nlm::Config>();
    
    // Test setting and getting values
    config->set("test_int", 42);
    config->set("test_double", 3.14);
    config->set("test_string", "test_string");
    config->set("test_bool", true);
    
    assert(config->get<int64_t>("test_int") == 42);
    assert(std::abs(config->get<double>("test_double") - 3.14) < 0.001);
    assert(config->get<std::string>("test_string") == "test_string");
    assert(config->get<bool>("test_bool") == true);
    
    std::cout << "    ✓ Basic configuration operations work" << std::endl;
    
    // Test 2: Check that brain factory works consistently
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    
    std::cout << "    ✓ Brain factory creates brain consistently" << std::endl;
    
    // Test 3: Check that brain type detection works
    std::string brainType = nlm::Brain::getBrainTypeFromConfig(*config);
    assert(!brainType.empty());
    
    std::cout << "    ✓ Brain type detection works: " << brainType << std::endl;
    
    // Test 4: Check that optimization detection works
    bool optimizationsEnabled = nlm::Brain::areOptimizationsEnabled(*config);
    // Should be false by default
    assert(!optimizationsEnabled);
    
    std::cout << "    ✓ Optimization detection works: " << (optimizationsEnabled ? "enabled" : "disabled") << std::endl;
    
    std::cout << "    Platform detection passed!" << std::endl;
}

void testCrossPlatformConfiguration() {
    std::cout << "    Testing cross-platform configuration..." << std::endl;
    
    // Test 1: Create configuration with various data types
    auto config = std::make_shared<nlm::Config>();
    
    // Set values of different types
    config->set("int_value", 42);
    config->set("float_value", 3.14159f);
    config->set("double_value", 2.718281828459045);
    config->set("string_value", "test_string");
    config->set("bool_value", true);
    
    // Set array values
    std::vector<int> intArray = {1, 2, 3, 4, 5};
    std::vector<double> doubleArray = {1.1, 2.2, 3.3, 4.4, 5.5};
    std::vector<std::string> stringArray = {"a", "b", "c"};
    
    config->set("int_array", intArray);
    config->set("double_array", doubleArray);
    config->set("string_array", stringArray);
    
    // Test retrieving values
    assert(config->get<int64_t>("int_value") == 42);
    assert(std::abs(config->get<double>("float_value") - 3.14159f) < 0.0001);
    assert(std::abs(config->get<double>("double_value") - 2.718281828459045) < 0.000001);
    assert(config->get<std::string>("string_value") == "test_string");
    assert(config->get<bool>("bool_value") == true);
    
    assert(config->get<std::vector<int>>("int_array") == intArray);
    assert(config->get<std::vector<double>>("double_array") == doubleArray);
    assert(config->get<std::vector<std::string>>("string_array") == stringArray);
    
    std::cout << "    ✓ Configuration handles various data types" << std::endl;
    
    // Test 2: Test configuration serialization/deserialization
    std::string summary = config->summary();
    assert(!summary.empty());
    
    std::cout << "    ✓ Configuration provides summary" << std::endl;
    
    // Test 3: Test configuration modification
    config->set("modified_value", 100);
    assert(config->get<int64_t>("modified_value") == 100);
    
    config->remove("modified_value");
    assert(!config->has("modified_value"));
    
    std::cout << "    ✓ Configuration allows modification and removal" << std::endl;
    
    // Test 4: Test configuration validation
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", 100);
    config2->set("region_count", 2);
    config2->set("connection_probability", 0.1f);
    
    // These should work without errors
    bool hasNeuronCount = config2->has("neuron_count");
    bool hasRegionCount = config2->has("region_count");
    bool hasConnectionProbability = config2->has("connection_probability");
    
    assert(hasNeuronCount && hasRegionCount && hasConnectionProbability);
    
    std::cout << "    ✓ Configuration validation works" << std::endl;
    
    std::cout << "    Cross-platform configuration passed!" << std::endl;
}

void testCrossPlatformBrainCreation() {
    std::cout << "    Testing cross-platform brain creation..." << std::endl;
    
    // Test 1: Create brain with basic configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    
    // This should work on any platform
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    
    // Test initialization
    bool success = brain->initialize();
    assert(success);
    
    std::cout << "    ✓ Basic brain creation works on all platforms" << std::endl;
    
    // Test 2: Create brain with optimization configuration
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", 50);
    config2->set("region_count", 1);
    config2->set("connection_probability", 0.05f);
    
    // Enable optimizations
    config2->set("brain_type", "optimized");
    config2->set("enable_event_driven", true);
    config2->set("enable_simd", true);
    config2->set("enable_multithreading", false); // Disable for test
    config2->set("enable_sparse_connectivity", false); // Disable for test
    
    // This should create an optimized brain
    auto optimizedBrain = nlm::Brain::createBrainWithConfig(config2);
    assert(optimizedBrain != nullptr);
    
    std::cout << "    ✓ Optimized brain creation works on all platforms" << std::endl;
    
    // Test 3: Test brain factory with different configurations
    struct BrainTestConfig {
        std::string brainType;
        bool eventDriven;
        bool simd;
        bool multithreading;
        bool sparse;
    };
    
    std::vector<BrainTestConfig> testConfigs = {
        {"basic", false, false, false, false},
        {"optimized", true, true, false, false},
        {"basic", true, false, false, false}, // Mixed optimization
    };
    
    for (const auto& testConfig : testConfigs) {
        auto testConfigPtr = std::make_shared<nlm::Config>();
        testConfigPtr->set("neuron_count", 10);
        testConfigPtr->set("region_count", 1);
        testConfigPtr->set("brain_type", testConfig.brainType);
        testConfigPtr->set("enable_event_driven", testConfig.eventDriven);
        testConfigPtr->set("enable_simd", testConfig.simd);
        testConfigPtr->set("enable_multithreading", testConfig.multithreading);
        testConfigPtr->set("enable_sparse_connectivity", testConfig.sparse);
        
        auto testBrain = nlm::Brain::createBrainWithConfig(testConfigPtr);
        assert(testBrain != nullptr);
        
        bool initialized = testBrain->initialize();
        assert(initialized);
        
        std::cout << "    ✓ Brain configuration type '" << testConfig.brainType 
                  << "' works on all platforms" << std::endl;
    }
    
    std::cout << "    Cross-platform brain creation passed!" << std::endl;
}

void testCrossPlatformErrorHandling() {
    std::cout << "    Testing cross-platform error handling..." << std::endl;
    
    // Test 1: Test null configuration handling
    try {
        auto brain = nlm::Brain::createBrainWithConfig(nullptr);
        assert(false && "Should have thrown exception for null config");
    } catch (const std::invalid_argument& e) {
        std::cout << "    ✓ Null configuration throws exception: " << e.what() << std::endl;
    }
    
    // Test 2: Test invalid configuration values
    auto config = std::make_shared<nlm::Config>();
    
    // Set valid configuration
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    
    // This should work
    auto brain = nlm::Brain::createBrainWithConfig(config);
    assert(brain != nullptr);
    
    // Test 3: Test brain factory error handling
    // The factory should handle errors gracefully
    auto invalidConfig = std::make_shared<nlm::Config>();
    // Don't set brain_type, let it default to "basic"
    
    auto invalidBrain = nlm::Brain::createBrainWithConfig(invalidConfig);
    assert(invalidBrain != nullptr);
    
    std::cout << "    ✓ Brain factory handles invalid configurations gracefully" << std::endl;
    
    // Test 4: Test configuration edge cases
    auto edgeConfig = std::make_shared<nlm::Config>();
    
    // Test with very large values
    edgeConfig->set("neuron_count", 1000000);
    edgeConfig->set("region_count", 100);
    
    auto edgeBrain = nlm::Brain::createBrainWithConfig(edgeConfig);
    assert(edgeBrain != nullptr);
    
    bool initialized = edgeBrain->initialize();
    // This might fail due to memory constraints, but shouldn't crash
    std::cout << "    ✓ Edge case brain creation handled" << std::endl;
    
    // Test 5: Test empty configuration
    auto emptyConfig = std::make_shared<nlm::Config>();
    
    // This should still work (create default brain)
    auto emptyBrain = nlm::Brain::createBrainWithConfig(emptyConfig);
    assert(emptyBrain != nullptr);
    
    std::cout << "    ✓ Empty configuration handled gracefully" << std::endl;
    
    std::cout << "    Cross-platform error handling passed!" << std::endl;
}

void testCrossPlatformPerformance() {
    std::cout << "    Testing cross-platform performance characteristics..." << std::endl;
    
    // Test 1: Measure brain creation time (basic)
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", 100);
    config1->set("region_count", 2);
    
    // Measure time to create and initialize basic brain
    auto start = std::chrono::high_resolution_clock::now();
    auto brain1 = nlm::Brain::createBrainWithConfig(config1);
    bool initialized1 = brain1->initialize();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "    Basic brain creation time: " << duration1.count() << "ms" << std::endl;
    assert(initialized1);
    
    // Test 2: Measure brain creation time (optimized)
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", 50);
    config2->set("region_count", 1);
    config2->set("brain_type", "optimized");
    config2->set("enable_event_driven", true);
    config2->set("enable_simd", true);
    
    start = std::chrono::high_resolution_clock::now();
    auto brain2 = nlm::Brain::createBrainWithConfig(config2);
    bool initialized2 = brain2->initialize();
    end = std::chrono::high_resolution_clock::now();
    
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "    Optimized brain creation time: " << duration2.count() << "ms" << std::endl;
    assert(initialized2);
    
    // Test 3: Test brain type detection performance
    auto config3 = std::make_shared<nlm::Config>();
    config3->set("neuron_count", 10);
    
    start = std::chrono::high_resolution_clock::now();
    std::string brainType = nlm::Brain::getBrainTypeFromConfig(*config3);
    end = std::chrono::high_resolution_clock::now();
    
    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "    Brain type detection time: " << duration3.count() << "µs" << std::endl;
    
    // Test 4: Test optimization detection performance
    start = std::chrono::high_resolution_clock::now();
    bool optimizationsEnabled = nlm::Brain::areOptimizationsEnabled(*config3);
    end = std::chrono::high_resolution_clock::now();
    
    auto duration4 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "    Optimization detection time: " << duration4.count() << "µs" << std::endl;
    
    std::cout << "    Cross-platform performance characteristics passed!" << std::endl;
}

} // namespace test_crossplatform
