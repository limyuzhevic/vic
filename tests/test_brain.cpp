// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "nlm/test_framework.h"
#include <iostream>

namespace test_brain {

TEST_CASE(testBrainCreation) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
}

TEST_CASE(testBrainInitialization) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    ASSERT_TRUE(brain.initialize());
    
    // Verify brain initialized with expected properties
    ASSERT_EQ(brain.getRegionCount(), 2);
    ASSERT_EQ(brain.getTotalNeuronCount(), 100);
    ASSERT_TRUE(brain.getTotalSynapseCount() > 0);
}

TEST_CASE(testBrainStep) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    ASSERT_EQ(brain.getTotalNeuronCount(), 100);
}

TEST_CASE(testBrainReset) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    ASSERT_EQ(brain.getTotalNeuronCount(), 50);
}

TEST_CASE(testBrainRegions) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    ASSERT_EQ(regionIds.size(), 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        ASSERT_TRUE(region != nullptr);
    }
}

TEST_CASE(testBrainActionProduction) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    ASSERT_TRUE(action != nullptr);
}

TEST_CASE(testBrainStatus) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    ASSERT_TRUE(brain.getTotalNeuronCount() > 0);
    ASSERT_TRUE(brain.getTotalSynapseCount() >= 0);
    ASSERT_TRUE(brain.getFiringNeuronCount() >= 0);
    ASSERT_TRUE(brain.getAverageFiringRate() >= 0.0f);
}

std::vector<TestResult> runAllTests() {
    return test_brain::runAllTests();
}

} // namespace test_brain

// Simple test runner for backward compatibility
void runAll() {
    auto results = test_brain::runAllTests();
    printTestSummary(results);
}
