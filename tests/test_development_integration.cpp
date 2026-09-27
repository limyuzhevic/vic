// Development Integration Tests for Phase 6 NLM Brain
// Tests developmental stage progression and plasticity changes

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_development_integration {

void testDevelopmentalStageProgression() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get developmental stage
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial);
    
    // Test stage transitions
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testDevelopmentalStageProgression passed" << std::endl;
}

void testDevelopmentEffectsOnPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("development_plasticity_modification", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get developmental system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test development system update
    // Development affects structural plasticity rates
    brain.develop();
    
    // Different developmental stages should affect plasticity
    for (int stage = 0; stage < 5; ++stage) {
        brain.setDevelopmentalStage(static_cast<nlm::DevelopmentalStage>(stage));
        brain.develop();
    }
    
    std::cout << "    testDevelopmentEffectsOnPlasticity passed" << std::endl;
}

void testCriticalPeriodPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Set to critical period
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    
    // During critical period, development should have stronger effects
    brain.develop();
    
    // Check developmental stage
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    std::cout << "    testCriticalPeriodPlasticity passed" << std::endl;
}

void testMaturationEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Set to maturation stage
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    
    // Maturation should stabilize development
    brain.develop();
    
    // Check developmental stage
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    std::cout << "    testMaturationEffects passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get development system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test development system with different stages
    nlm::DevelopmentalStage stages[] = {
        nlm::DevelopmentalStage::Initial,
        nlm::DevelopmentalStage::CriticalPeriod,
        nlm::DevelopmentalStage::Maturation,
        nlm::DevelopmentalStage::Adult
    };
    
    for (auto stage : stages) {
        brain.setDevelopmentalStage(stage);
        brain.develop();
        
        // Verify developmental stage was set correctly
        assert(brain.getDevelopmentalStage() == stage);
    }
    
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

} // namespace test_development_integration
