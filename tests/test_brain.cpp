// Brain Tests
// Enhanced with NLM Test Framework
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <string>

// Test macros for NLM Test Framework
#define NLM_TEST_CASE_LEGACY(name, testFunc) \
    void name##TestLegacy() { testFunc; } \
    NLM_TEST_CASE(name, name##TestLegacy)

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    NLM_ASSERT_TRUE(success);
    
    // Verify brain initialized with expected properties
    NLM_ASSERT_EQ(brain.getRegionCount(), 2);
    NLM_ASSERT_EQ(brain.getTotalNeuronCount(), 100);
    NLM_ASSERT_GT(brain.getTotalSynapseCount(), 0);
    
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
    
    // Just verify it doesn't crash
    NLM_ASSERT_EQ(brain.getTotalNeuronCount(), 100);
    
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
    NLM_ASSERT_EQ(brain.getTotalNeuronCount(), 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    NLM_ASSERT_EQ(regionIds.size(), 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        NLM_ASSERT_NOT_NULL(region);
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
    NLM_ASSERT_NOT_NULL(action);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    NLM_ASSERT_GT(brain.getTotalNeuronCount(), 0);
    NLM_ASSERT_GE(brain.getTotalSynapseCount(), 0);
    NLM_ASSERT_GE(brain.getFiringNeuronCount(), 0);
    NLM_ASSERT_GE(brain.getAverageFiringRate(), 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

// Modern test with enhanced brain features
void testBrainEnhancedFeatures() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    NLM_ASSERT_TRUE(success);
    
    // Test enhanced features
    NLM_ASSERT_EQ(brain.getRegionCount(), 5);
    NLM_ASSERT_EQ(brain.getTotalNeuronCount(), 200);
    NLM_ASSERT_GT(brain.getTotalSynapseCount(), 100);
    
    // Test brain properties
    auto regionIds = brain.getRegionIds();
    NLM_ASSERT_EQ(regionIds.size(), 5);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        NLM_ASSERT_NOT_NULL(region);
    }
    
    // Test action production
    auto action = brain.produceAction();
    NLM_ASSERT_NOT_NULL(action);
    
    std::cout << "    testBrainEnhancedFeatures passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Brain tests..." << std::endl;
    
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainEnhancedFeatures();
}

} // namespace test_brain
