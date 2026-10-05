// Regression Tests
// Phase 3.5: Backward compatibility and regression testing

#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_regression {

void testBackwardCompatibility() {
    // Test backward compatibility with existing API
    nlm::Config config;
    
    // Test old-style API (based on existing tests)
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Verify the old API still works
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.01);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test getOr method
    int defaultVal = config.getOr<int>("missing", 100);
    assert(defaultVal == 100);
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void testExistingAPIFunctionality() {
    // Test existing API functionality
    nlm::Config config;
    
    // Test basic configuration operations
    config.set("test_key", 123);
    assert(config.has("test_key"));
    
    auto value = config.get<int>("test_key");
    assert(value.has_value() && *value == 123);
    
    config.set("test_key", 456);
    assert(config.getOr<int>("test_key", 0) == 456);
    
    config.remove("test_key");
    assert(!config.has("test_key"));
    
    // Test keys functionality
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    // Test summary functionality
    std::string summary = config.summary();
    assert(!summary.empty());
    
    // Test Brain existing API
    auto brainConfig = std::make_shared<nlm::Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(brainConfig);
    assert(brain.initialize());
    
    // Test existing Brain methods
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test existing neuron functionality
    nlm::Neuron neuron(nlm::NeuronId(1));
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    std::cout << "    testExistingAPIFunctionality passed" << std::endl;
}

void testpImplPatternBehavior() {
    // Test pImpl pattern behavior (pointer to implementation)
    nlm::Config config;
    
    // Config should manage its internal implementation
    config.set("test_value", 42);
    assert(config.has("test_value"));
    
    // Config should handle memory management transparently
    config.clear();
    assert(!config.has("test_value"));
    
    // Test that operations don't crash or cause issues
    for (int i = 0; i < 10; ++i) {
        config.set(std::to_string(i), i);
        assert(config.getOr<int>(std::to_string(i), -1) == i);
    }
    
    // Clean up
    config.clear();
    assert(config.getKeys().empty());
    
    std::cout << "    testpImplPatternBehavior passed" << std::endl;
}

void testTemplateSpecializations() {
    // Test template specializations
    nlm::Config config;
    
    // Test different type specializations
    config.set("int_special", 123);
    auto intValue = config.get<int>("int_special");
    assert(intValue.has_value() && *intValue == 123);
    
    config.set("double_special", 3.14);
    auto doubleValue = config.get<double>("double_special");
    assert(doubleValue.has_value());
    
    config.set("string_special", std::string("test"));
    auto stringValue = config.get<std::string>("string_special");
    assert(stringValue.has_value() && *stringValue == "test");
    
    config.set("bool_special", true);
    auto boolValue = config.get<bool>("bool_special");
    assert(boolValue.has_value() && *boolValue == true);
    
    // Test arrays
    std::vector<int> intArray{1, 2, 3, 4, 5};
    config.set("array_special", intArray);
    auto retrievedArray = config.get<std::vector<int>>("array_special");
    assert(retrievedArray.has_value() && retrievedArray->size() == 5);
    
    // Test Brain template behavior (if applicable)
    auto brainConfig = std::make_shared<nlm::Config>();
    brainConfig->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(brainConfig);
    assert(brain.initialize());
    
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    
    std::cout << "    testTemplateSpecializations passed" << std::endl;
}

void testRegressionFromPhase1_1() {
    // Test regression from Phase 1.1 improvements (Python bindings)
    // This would test the enhanced Python bindings from Phase 1.1
    auto config = std::make_shared<nlm::Config>();
    
    // Test enhanced config functionality
    config->set("python_compat_test", 123);
    assert(config->has("python_compat_test"));
    
    // Test enhanced error handling
    auto missingValue = config->get<int>("missing_key");
    assert(!missingValue.has_value());
    
    // Test enhanced type safety
    config->set("type_safe_test", std::string("test_string"));
    auto retrievedValue = config->get<std::string>("type_safe_test");
    assert(retrievedValue.has_value() && *retrievedValue == "test_string");
    
    std::cout << "    testRegressionFromPhase1_1 passed" << std::endl;
}

void testRegressionFromPhase1_2() {
    // Test regression from Phase 1.2 improvements (enhanced Python features)
    auto config = std::make_shared<nlm::Config>();
    
    // Test enhanced Python-specific utilities
    config->set("numpy_compat_test", std::vector<double>{1.1, 2.2, 3.3});
    auto numpyValue = config->get<std::vector<double>>("numpy_compat_test");
    assert(numpyValue.has_value() && numpyValue->size() == 3);
    
    // Test enhanced error formatting
    config->set("error_test", "test_value");
    auto errorValue = config->get<std::string>("error_test");
    assert(errorValue.has_value() && *errorValue == "test_value");
    
    std::cout << "    testRegressionFromPhase1_2 passed" << std::endl;
}

void testRegressionFromPhase2() {
    // Test regression from Phase 2 improvements (new C++ features)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test new C++ features from Phase 2
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test enhanced neuron functionality from Phase 2
    nlm::Neuron neuron(nlm::NeuronId(1));
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getMembranePotential() == -70.0f); // Default value
    
    std::cout << "    testRegressionFromPhase2 passed" << std::endl;
}

void testRegressionFromPhase3() {
    // Test regression from Phase 3 improvements (enhanced features)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test Phase 3 enhanced features
    assert(brain.getTotalNeuronCount() == 150);
    assert(brain.getRegionCount() == 3);
    
    // Enable memory pool
    brain.enableMemoryPool(true);
    
    // Test reward modulation
    brain.applyRewardModulation(0.8f, 0.6f);
    
    // Step brain
    brain.step(0);
    
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testRegressionFromPhase3 passed" << std::endl;
}

void testRegressionCompleteSystem() {
    // Test complete system regression
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Complete system regression test
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        // Test integration of all Phase 3 features
        brain.applyRewardModulation(0.5f, 0.3f);
        brain.updateDevelopment(step);
        brain.step(step);
        
        // Verify system integrity
        assert(brain.getTotalNeuronCount() == 200);
        assert(brain.getRegionCount() == 4);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    std::cout << "    testRegressionCompleteSystem passed" << std::endl;
}

void runAll() {
    testBackwardCompatibility();
    testExistingAPIFunctionality();
    testpImplPatternBehavior();
    testTemplateSpecializations();
    testRegressionFromPhase1_1();
    testRegressionFromPhase1_2();
    testRegressionFromPhase2();
    testRegressionFromPhase3();
    testRegressionCompleteSystem();
}

} // namespace test_regression