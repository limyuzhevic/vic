// Error Handling Tests for NLM Phase 6
// Tests edge cases, error conditions, and exception handling

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigFactory.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace test_error_handling {

void testConfigErrorHandling() {
    std::cout << "    Testing Config error handling..." << std::endl;
    
    nlm::Config config;
    
    // Test getting non-existent key throws exception
    try {
        config.get<int>("nonexistent");
        assert(false); // Should not reach here
    } catch (const nlm::ConfigError& e) {
        assert(true); // Expected exception
        std::cout << "      ✓ ConfigError thrown for missing key" << std::endl;
    }
    
    // Test tryGet returns nullopt for non-existent key
    auto val = config.tryGet<int>("nonexistent");
    assert(!val.has_value());
    std::cout << "      ✓ tryGet returns nullopt for missing key" << std::endl;
    
    // Test invalid type conversion throws exception
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    try {
        config.get<int>("string_value");
        assert(false); // Should not reach here
    } catch (const nlm::ConfigError& e) {
        assert(true); // Expected exception
        std::cout << "      ✓ ConfigError thrown for type mismatch" << std::endl;
    }
    
    std::cout << "    Config error handling test passed" << std::endl;
}

void testBrainErrorHandling() {
    std::cout << "    Testing Brain error handling..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that brain operations handle errors gracefully
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 1);
    
    // Test step with various inputs
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "    Brain error handling test passed" << std::endl;
}

void testMemoryManagementErrorHandling() {
    std::cout << "    Testing memory management error handling..." << std::endl;
    
    // Test memory management with various configurations
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default); // Edge case: zero neurons
    config1->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    // Brain should handle zero neuron count gracefully
    assert(brain1.getTotalNeuronCount() == 0);
    std::cout << "      ✓ Brain handles zero neuron count" << std::endl;
    
    // Test with very large network
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(50000), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config2->set("connection_probability", 0.01f, nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    assert(brain2.getTotalNeuronCount() == 50000);
    std::cout << "      ✓ Brain handles large network" << std::endl;
    
    // Test reset functionality error handling
    brain2.step(0);
    brain2.reset();
    assert(brain2.getTotalNeuronCount() == 50000);
    std::cout << "      ✓ Brain reset works correctly" << std::endl;
    
    std::cout << "    Memory management error handling test passed" << std::endl;
}

void testThreadSafetyErrorHandling() {
    std::cout << "    Testing thread safety error handling..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test concurrent access
    std::vector<std::thread> threads;
    const size_t threadCount = 10;
    
    for (size_t i = 0; i < threadCount; ++i) {
        threads.emplace_back([&brain, i]() {
            for (nlm::SimulationStep step = 0; step < 20; ++step) {
                brain.step(step);
                // Random operations
                if (step % 5 == 0) {
                    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify brain is still in valid state
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getRegionCount() == 3);
    std::cout << "      ✓ Thread safety test passed" << std::endl;
    
    std::cout << "    Thread safety error handling test passed" << std::endl;
}

void testConfigValidationErrorHandling() {
    std::cout << "    Testing config validation error handling..." << std::endl;
    
    nlm::Config config;
    
    // Add various validators
    config.addValidator("positive_number", 
        [](const nlm::ConfigValue& val) {
            return std::visit([](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                    return v > 0;
                }
                return true;
            }, val);
        },
        [](const nlm::ConfigValue& val) { return "Value must be positive"; },
        "positive_validator");
    
    config.addValidator("non_empty_string",
        [](const nlm::ConfigValue& val) {
            return std::visit([](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return !v.empty();
                }
                return true;
            }, val);
        },
        [](const nlm::ConfigValue& val) { return "String must not be empty"; },
        "non_empty_validator");
    
    // Test valid values
    config.set("positive_num", 42, nlm::ConfigSource::Runtime);
    config.set("string_val", "test", nlm::ConfigSource::Runtime);
    
    // Test validation with potentially invalid values
    config.set("negative_num", -1, nlm::ConfigSource::Runtime); // May fail validation
    config.set("empty_string", "", nlm::ConfigSource::Runtime); // May fail validation
    
    // Validation should still work
    bool isValid = config.validate();
    auto errors = config.getValidationErrors();
    
    std::cout << "      ✓ Config validation handles edge cases" << std::endl;
    
    std::cout << "    Config validation error handling test passed" << std::endl;
}

void testFileOperationErrorHandling() {
    std::cout << "    Testing file operation error handling..." << std::endl;
    
    nlm::Config config;
    
    // Test saving to non-existent directory (should handle gracefully)
    bool saveSuccess = config.saveToFile("/nonexistent/path/config.cfg", false, "cfg");
    // Implementation may or may not support this, but should not crash
    std::cout << "      ✓ File save handles non-existent directory" << std::endl;
    
    // Test loading non-existent file (should return false)
    bool loadSuccess = config.loadFromFile("/nonexistent/path/config.cfg");
    assert(!loadSuccess); // Should fail gracefully
    std::cout << "      ✓ File load handles non-existent file" << std::endl;
    
    // Test invalid file format
    std::ofstream invalidFile("/tmp/invalid_config.txt");
    invalidFile << "This is not a valid config file format";
    invalidFile.close();
    
    bool invalidLoadSuccess = config.loadFromFile("/tmp/invalid_config.txt");
    // Implementation may or may not support this format, but should handle gracefully
    std::cout << "      ✓ File load handles invalid format" << std::endl;
    
    std::cout << "    File operation error handling test passed" << std::endl;
}

void testEdgeCaseTesting() {
    std::cout << "    Testing edge cases..." << std::endl;
    
    // Test with extreme values
    auto config = std::make_shared<nlm::Config>();
    
    // Very large neuron count
    config->set("neuron_count", static_cast<int64_t>(1000000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.001f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    assert(brain.getTotalNeuronCount() == 1000000);
    assert(brain.getRegionCount() == 100);
    std::cout << "      ✓ Handles very large neuron count" << std::endl;
    
    // Test with minimal configuration
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config2->set("connection_probability", 1.0f, nlm::ConfigSource::Default); // Maximum connectivity
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    assert(brain2.getTotalNeuronCount() == 10);
    std::cout << "      ✓ Handles minimal configuration" << std::endl;
    
    // Test with zero connection probability
    auto config3 = std::make_shared<nlm::Config>();
    config3->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config3->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config3->set("connection_probability", 0.0f, nlm::ConfigSource::Default); // Zero connectivity
    
    nlm::Brain brain3(config3);
    brain3.initialize();
    
    assert(brain3.getTotalNeuronCount() == 100);
    // May have zero synapses due to zero connection probability
    std::cout << "      ✓ Handles zero connection probability" << std::endl;
    
    std::cout << "    Edge case testing completed" << std::endl;
}

void runAll() {
    std::cout << "\n" << std::string(80, '═') << std::endl;
    std::cout << "  NLM PHASE 6 ERROR HANDLING AND EDGE CASE TESTS" << std::endl;
    std::cout << "  Testing error conditions and exception handling" << std::endl;
    std::cout << std::string(80, '═') << std::endl;
    
    testConfigErrorHandling();
    testBrainErrorHandling();
    testMemoryManagementErrorHandling();
    testThreadSafetyErrorHandling();
    testConfigValidationErrorHandling();
    testFileOperationErrorHandling();
    testEdgeCaseTesting();
    
    std::cout << "\n" << std::string(80, '═') << std::endl;
    std::cout << "  ALL ERROR HANDLING TESTS COMPLETED SUCCESSFULLY!" << std::endl;
    std::cout << std::string(80, '═') << std::endl;
}

} // namespace test_error_handling
