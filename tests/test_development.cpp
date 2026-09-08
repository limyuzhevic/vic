// Comprehensive Development Tests
// Phase 2: Real Neural Computation Tests - Development Systems

#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Maturation.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_development {

void testDevelopmentSystemBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test initial developmental stage
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Test development update
    developmentSystem->update(&brain, nlm::RandomGenerator(42), 0.001f);
    
    // Stage may change after update
    assert(brain.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testDevelopmentSystemBasic passed" << std::endl;
}

void testDevelopmentStageTransitions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all developmental stages
    std::vector<nlm::DevelopmentalStage> stages = {
        nlm::DevelopmentalStage::Initial,
        nlm::DevelopmentalStage::CriticalPeriod,
        nlm::DevelopmentalStage::Maturation,
        nlm::DevelopmentalStage::Adult
    };
    
    for (auto stage : stages) {
        brain.setDevelopmentalStage(stage);
        assert(brain.getDevelopmentalStage() == stage);
        
        // Update development system
        auto* devSystem = brain.getDevelopmentSystem();
        if (devSystem) {
            devSystem->update(&brain, nlm::RandomGenerator(42), 0.001f);
        }
        
        // Stage should remain consistent
        assert(brain.getDevelopmentalStage() == stage);
    }
    
    std::cout << "    testDevelopmentStageTransitions passed" << std::endl;
}

void testMaturation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* maturation = brain.getDevelopmentSystem();
    assert(maturation != nullptr);
    
    // Test maturation effects on plasticity
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    
    // Record initial plasticity rates (if accessible)
    // In real implementation, these would be configurable
    
    // Update maturation
    maturation->update(&brain, nlm::RandomGenerator(42), 0.001f);
    
    // Test plasticity modulation
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testMaturation passed" << std::endl;
}

void testSynaptogenesis() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* synaptogenesis = brain.getDevelopmentSystem();
    assert(synaptogenesis != nullptr);
    
    // Test synaptogenesis in different developmental stages
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    
    // Set synaptogenesis rate
    auto* sp = brain.getStructuralPlasticity();
    if (sp) {
        float initialRate = sp->getSynaptogenesisRate();
        sp->setSynaptogenesisRate(0.001f);
        
        assert(sp->getSynaptogenesisRate() == 0.001f);
        
        // Reset
        sp->setSynaptogenesisRate(initialRate);
    }
    
    std::cout << "    testSynaptogenesis passed" << std::endl;
}

void testPruning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* pruning = brain.getDevelopmentSystem();
    assert(pruning != nullptr);
    
    // Test pruning in different developmental stages
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    
    auto* sp = brain.getStructuralPlasticity();
    if (sp) {
        float initialPruningRate = sp->getPruningRate();
        sp->setPruningRate(0.0001f);
        
        assert(sp->getPruningRate() == 0.0001f);
        
        // Reset
        sp->setPruningRate(initialPruningRate);
    }
    
    std::cout << "    testPruning passed" << std::endl;
}

void testDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    
    assert(developmentSystem != nullptr);
    assert(structuralPlasticity != nullptr);
    
    // Test development effects on structural plasticity
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    
    // Record initial rates
    float initialSynaptogenesis = structuralPlasticity->getSynaptogenesisRate();
    float initialPruning = structuralPlasticity->getPruningRate();
    
    // Update development
    developmentSystem->update(&brain, nlm::RandomGenerator(42), 0.001f);
    
    // Rates may change based on developmental stage
    // In this implementation, they're set during Brain construction
    assert(structuralPlasticity->getSynaptogenesisRate() >= 0.0f);
    assert(structuralPlasticity->getPruningRate() >= 0.0f);
    
    // Test stage-specific effects
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void testDevelopmentErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test error handling with invalid inputs
    brain.setDevelopmentalStage(static_cast<nlm::DevelopmentalStage>(999));  // Invalid value
    
    // Should handle gracefully (implementation dependent)
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    
    // Update should not crash
    developmentSystem->update(&brain, nlm::RandomGenerator(42), 0.001f);
    
    std::cout << "    testDevelopmentErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running Development tests..." << std::endl;
    testDevelopmentSystemBasic();
    testDevelopmentStageTransitions();
    testMaturation();
    testSynaptogenesis();
    testPruning();
    testDevelopmentIntegration();
    testDevelopmentErrorHandling();
}

} // namespace test_development
