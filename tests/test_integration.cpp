// Integration Tests - Testing complete brain system
#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_integration {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify basic properties still hold
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSpikeCount() >= 0);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testConfigFileOperations() {
    nlm::Config config;
    
    // Test setting and getting values
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "hello world");
    config.set("test_bool", true);
    
    // Verify values
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.01);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello world");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test getOr with missing value
    int defaultVal = config.getOr<int>("missing", 999);
    assert(defaultVal == 999);
    
    // Test removal
    config.remove("test_int");
    assert(!config.has("test_int"));
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void testRandomGeneratorEdgeCases() {
    nlm::RandomGenerator rng(42);
    
    // Test boundary conditions
    uint32_t result = rng.uniformInt(5, 5);
    assert(result == 5);
    
    // Test invalid ranges (should throw)
    try {
        rng.uniformInt(10, 5);
        assert(false); // Should not reach here
    } catch (const std::invalid_argument& e) {
        // Expected
    }
    
    try {
        rng.uniformReal(1.0, 0.0);
        assert(false); // Should not reach here
    } catch (const std::invalid_argument& e) {
        // Expected
    }
    
    std::cout << "    testRandomGeneratorEdgeCases passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testConfigFileOperations();
    testRandomGeneratorEdgeCases();
}

} // namespace test_integration
