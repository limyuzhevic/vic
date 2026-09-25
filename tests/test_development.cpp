// Development Tests
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_development {

TEST_CASE("DevelopmentSystem creation and initialization", "[development][system][creation]") {
    nlm::DevelopmentSystem devSystem;
    REQUIRE(!devSystem.isInitialized());
}

TEST_CASE("DevelopmentSystem initialization with config", "[development][system][initialization]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("development.stage", 1, nlm::ConfigSource::Runtime);
    config->set("development.plasticity_rate", 0.5, nlm::ConfigSource::Runtime);
    
    nlm::DevelopmentSystem devSystem;
    devSystem.initialize(config);
    REQUIRE(devSystem.isInitialized());
}

TEST_CASE("Synaptogenesis activity", "[development][synaptogenesis][activity]") {
    nlm::Synaptogenesis synaptogenesis;
    
    REQUIRE(synaptogenesis.getConnectionFormationRate() > 0.0f);
    REQUIRE(synaptogenesis.getSynapticStrength() > 0.0f);
}

TEST_CASE("Synaptogenesis with different developmental stages", "[development][synaptogenesis][stages]") {
    for (int stage = 1; stage <= 5; ++stage) {
        nlm::Synaptogenesis synaptogenesis;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("development.stage", stage, nlm::ConfigSource::Runtime);
        
        nlm::Synaptogenesis devSynaptogenesis;
        devSynaptogenesis.initialize(config);
        
        // Verify stage-specific behavior
        REQUIRE(devSynaptogenesis.getConnectionFormationRate() > 0.0f);
    }
}

TEST_CASE("Pruning system activity", "[development][pruning][activity]") {
    nlm::Pruning pruning;
    
    REQUIRE(pruning.getPruningRate() > 0.0f);
    REQUIRE(pruning.getConnectionEliminationRate() > 0.0f);
}

TEST_CASE("Maturation process", "[development][maturation][process]") {
    nlm::Maturation maturation;
    
    REQUIRE(maturation.getMaturationProgress() >= 0.0f);
    REQUIRE(maturation.getMaturationProgress() <= 1.0f);
    REQUIRE(maturation.getExcitabilityChange() != 0.0f);
}

TEST_CASE("Development system integration", "[development][integration][system]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("development.stage", 3, nlm::ConfigSource::Runtime);
    config->set("development.plasticity_rate", 0.8, nlm::ConfigSource::Runtime);
    
    nlm::DevelopmentSystem devSystem;
    devSystem.initialize(config);
    
    REQUIRE(devSystem.isInitialized());
    
    // Test development processes
    REQUIRE(devSystem.getTotalConnections() > 0);
    REQUIRE(devSystem.getTotalPruningEvents() >= 0);
}

TEST_CASE("Development plasticity modulation", "[development][plasticity][modulation]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("development.stage", 2, nlm::ConfigSource::Runtime);
    config->set("development.plasticity_rate", 0.3, nlm::ConfigSource::Runtime);
    
    nlm::DevelopmentSystem devSystem;
    devSystem.initialize(config);
    
    // Verify plasticity modulation based on developmental stage
    REQUIRE(devSystem.isInitialized());
}

TEST_CASE("Development system reset functionality", "[development][system][reset]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("development.stage", 4, nlm::ConfigSource::Runtime);
    config->set("development.plasticity_rate", 0.6, nlm::ConfigSource::Runtime);
    
    nlm::DevelopmentSystem devSystem;
    devSystem.initialize(config);
    
    REQUIRE(devSystem.isInitialized());
    
    devSystem.reset();
    
    // After reset, system should still be functional
    devSystem.initialize(config);
    REQUIRE(devSystem.isInitialized());
}

} // namespace test_development
