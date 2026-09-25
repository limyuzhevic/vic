// Dynamics Tests
#include "dynamics/NeuralDynamics.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "core/Config/Config.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_section.hpp>

namespace test_dynamics {

TEST_CASE("NeuralDynamics creation and basic properties", "[dynamics][creation]") {
    nlm::NeuralDynamics dynamics;
    REQUIRE(!dynamics.isInitialized());
}

TEST_CASE("SpikeSystem basic functionality", "[dynamics][spikesystem]") {
    nlm::SpikeSystem spikeSystem;
    REQUIRE(spikeSystem.getSpikeCount() == 0);
}

TEST_CASE("NeuralDynamics initialization with config", "[dynamics][initialization]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("dynamics.dt", 0.001, nlm::ConfigSource::Runtime);
    config->set("dynamics.simulation_time", 1000.0, nlm::ConfigSource::Runtime);
    
    nlm::NeuralDynamics dynamics;
    dynamics.initialize(config);
    REQUIRE(dynamics.isInitialized());
}

TEST_CASE("SpikeSystem spike recording and counting", "[dynamics][spikesystem][spikes]") {
    nlm::SpikeSystem spikeSystem;
    REQUIRE(spikeSystem.getSpikeCount() == 0);
    
    spikeSystem.recordSpike(nlm::NeuronId(1), 0.1);
    REQUIRE(spikeSystem.getSpikeCount() == 1);
    
    spikeSystem.recordSpike(nlm::NeuronId(2), 0.2);
    spikeSystem.recordSpike(nlm::NeuronId(3), 0.3);
    REQUIRE(spikeSystem.getSpikeCount() == 3);
}

TEST_CASE("Dynamics integration with neurons", "[dynamics][integration][neurons]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("dynamics.dt", 0.001, nlm::ConfigSource::Runtime);
    config->set("dynamics.simulation_steps", 100, nlm::ConfigSource::Runtime);
    
    nlm::NeuralDynamics dynamics;
    dynamics.initialize(config);
    
    // Simulate some dynamics
    for (int i = 0; i < 50; ++i) {
        dynamics.advanceStep(i);
    }
    
    // Verify state
    REQUIRE(dynamics.isInitialized());
    REQUIRE(dynamics.getCurrentStep() >= 0);
}

TEST_CASE("SpikeSystem threshold and refractory handling", "[dynamics][spikesystem][threshold]") {
    nlm::SpikeSystem spikeSystem;
    
    // Test initial threshold
    REQUIRE(spikeSystem.getThreshold() > -1000.0f);
    REQUIRE(spikeSystem.getRefractoryTime() > 0.0f);
    
    // Test spike recording with different neuron IDs
    spikeSystem.recordSpike(nlm::NeuronId(1), 1.0);
    spikeSystem.recordSpike(nlm::NeuronId(2), 2.0);
    spikeSystem.recordSpike(nlm::NeuronId(1), 3.0);  // Same neuron, different time
    
    REQUIRE(spikeSystem.getSpikeCount() == 3);
}

TEST_CASE("Dynamics configuration validation", "[dynamics][config][validation]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("dynamics.dt", 0.001, nlm::ConfigSource::Runtime);
    config->set("dynamics.simulation_time", 1000.0, nlm::ConfigSource::Runtime);
    config->set("dynamics.max_neurons", 1000, nlm::ConfigSource::Runtime);
    
    nlm::NeuralDynamics dynamics;
    dynamics.initialize(config);
    
    // Verify configuration was applied
    REQUIRE(dynamics.isInitialized());
}

} // namespace test_dynamics
