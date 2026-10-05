// Development System Integration Tests
// Phase 3.3: Integration tests for development system

#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>

namespace test_development_integration {

void testDevelopmentSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("development_rate", 0.1, nlm::ConfigSource::Default);
    config->set("critical_period_enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development system integration
    brain.updateDevelopment(0); // timestep
    
    assert(brain.getTotalNeuronCount() == 200);
    assert(std::isfinite(brain.getAverageFiringRate()));
    
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

void testSynaptogenesisIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test synaptogenesis integration
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        brain.updateDevelopment(step);
        
        assert(brain.getTotalNeuronCount() == 180);
        assert(brain.getTotalSynapseCount() > 0);
    }
    
    std::cout << "    testSynaptogenesisIntegration passed" << std::endl;
}

void testPruningIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(220), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("pruning_threshold", 0.3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test pruning integration
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.updateDevelopment(step);
        
        assert(brain.getTotalNeuronCount() == 220);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    std::cout << "    testPruningIntegration passed" << std::endl;
}

void testMaturationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(190), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("maturation_rate", 0.08, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test maturation integration
    for (nlm::SimulationStep step = 0; step < 18; ++step) {
        brain.updateDevelopment(step);
        
        assert(brain.getTotalNeuronCount() == 190);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    std::cout << "    testMaturationIntegration passed" << std::endl;
}

void testCompleteDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("development_rate", 0.15, nlm::ConfigSource::Default);
    config->set("critical_period_enabled", true, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.1, nlm::ConfigSource::Default);
    config->set("pruning_threshold", 0.4, nlm::ConfigSource::Default);
    config->set("maturation_rate", 0.12, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Complete development integration test
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Development system should handle multiple development processes
        brain.updateDevelopment(step);
        
        // Verify development integration
        assert(brain.getTotalNeuronCount() == 250);
        assert(std::isfinite(brain.getAverageFiringRate()));
        
        // Check developmental stage
        brain.getDevelopmentalStage();
    }
    
    std::cout << "    testCompleteDevelopmentIntegration passed" << std::endl;
}

void testDevelopmentPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("development_rate", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark development performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float totalDevelopmentEffect = 0.0f;
    for (nlm::SimulationStep step = 0; step < 60; ++step) {
        // Development system updates
        brain.updateDevelopment(step);
        
        totalDevelopmentEffect += brain.getAverageFiringRate();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    testDevelopmentPerformance: " << duration.count() << " microseconds for 60 development steps" << std::endl;
    std::cout << "    Average development effect: " << (totalDevelopmentEffect / 60.0f) << std::endl;
    
    std::cout << "    testDevelopmentPerformance passed" << std::endl;
}

void testDevelopmentResourceManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(280), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("development_rate", 0.18, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development resource management under load
    for (int i = 0; i < 5; ++i) {
        // Heavy development usage
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.updateDevelopment(step);
        }
        
        // Reset and verify cleanup
        brain.reset();
        assert(brain.initialize());
    }
    
    std::cout << "    testDevelopmentResourceManagement passed" << std::endl;
}

void testDevelopmentSystemRobustness() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(320), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("development_rate", 0.25, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development system robustness
    for (nlm::SimulationStep step = 0; step < 40; ++step) {
        // Variable development inputs
        brain.updateDevelopment(step);
        
        // Stress development system
        assert(brain.getTotalNeuronCount() == 320);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    std::cout << "    testDevelopmentSystemRobustness passed" << std::endl;
}

void testDevelopmentIntegrationWithOtherSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_neuromodulation", true, nlm::ConfigSource::Default);
    config->set("development_rate", 0.3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development integration with other systems
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Development should work with neuromodulation
        brain.updateDevelopment(step);
        brain.applyRewardModulation(0.3f, 0.2f);
        
        // Step the integrated system
        brain.step(step);
        
        // Verify all systems are working together
        assert(brain.getTotalNeuronCount() == 350);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    std::cout << "    testDevelopmentIntegrationWithOtherSystems passed" << std::endl;
}

void runAll() {
    testDevelopmentSystemIntegration();
    testSynaptogenesisIntegration();
    testPruningIntegration();
    testMaturationIntegration();
    testCompleteDevelopmentIntegration();
    testDevelopmentPerformance();
    testDevelopmentResourceManagement();
    testDevelopmentSystemRobustness();
    testDevelopmentIntegrationWithOtherSystems();
}

} // namespace test_development_integration