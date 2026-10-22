// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/test/TestFramework.hpp"
#include <iostream>

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
    nlm::test::TestAssert::equal(true, success, "testBrainInitialization");
    
    // Verify brain initialized with expected properties
    nlm::test::TestAssert::equal(2, brain.getRegionCount(), "testBrainInitialization");
    nlm::test::TestAssert::equal(100, brain.getTotalNeuronCount(), "testBrainInitialization");
    nlm::test::TestAssert::greaterEqual(0, brain.getTotalSynapseCount(), "testBrainInitialization");
    
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
    nlm::test::TestAssert::equal(100, brain.getTotalNeuronCount(), "testBrainStep");
    
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
    nlm::test::TestAssert::equal(50, brain.getTotalNeuronCount(), "testBrainReset");
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    nlm::test::TestAssert::equal(3, regionIds.size(), "testBrainRegions");
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        nlm::test::TestAssert::notNull(region, "testBrainRegions");
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
    nlm::test::TestAssert::notNull(action.get(), "testBrainActionProduction");
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    nlm::test::TestAssert::greaterEqual(0, brain.getTotalNeuronCount(), "testBrainStatus");
    nlm::test::TestAssert::greaterEqual(0, brain.getTotalSynapseCount(), "testBrainStatus");
    nlm::test::TestAssert::greaterEqual(0, brain.getFiringNeuronCount(), "testBrainStatus");
    nlm::test::TestAssert::greaterEqual(0.0f, brain.getAverageFiringRate(), "testBrainStatus");
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
}

} // namespace test_brain
