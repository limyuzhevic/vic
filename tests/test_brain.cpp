// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_brain {

TEST_CASE("Brain creation with configuration", "[brain][creation][config]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
}

TEST_CASE("Brain initialization and properties", "[brain][initialization]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    REQUIRE(brain.initialize());
    
    // Verify brain initialized with expected properties
    REQUIRE(brain.getRegionCount() == 2);
    REQUIRE(brain.getTotalNeuronCount() == 100);
    REQUIRE(brain.getTotalSynapseCount() > 0);
}

TEST_CASE("Brain step simulation", "[brain][step][simulation]") {
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
    REQUIRE(brain.getTotalNeuronCount() == 100);
}

TEST_CASE("Brain reset operation", "[brain][reset]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    REQUIRE(brain.getTotalNeuronCount() == 50);
}

TEST_CASE("Brain regions management", "[brain][regions]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    REQUIRE(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        REQUIRE(region != nullptr);
    }
}

TEST_CASE("Brain action production", "[brain][action]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    REQUIRE(action != nullptr);
}

TEST_CASE("Brain status and statistics", "[brain][status][statistics]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    REQUIRE(brain.getTotalNeuronCount() > 0);
    REQUIRE(brain.getTotalSynapseCount() >= 0);
    REQUIRE(brain.getFiringNeuronCount() >= 0);
    REQUIRE(brain.getAverageFiringRate() >= 0.0f);
}

TEST_CASE("Brain with different region counts", "[brain][regions][parametrized]") {
    for (int regionCount = 1; regionCount <= 5; ++regionCount) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        REQUIRE(brain.initialize());
        REQUIRE(brain.getRegionCount() == regionCount);
        REQUIRE(brain.getTotalNeuronCount() == 50);
    }
}

} // namespace test_brain
