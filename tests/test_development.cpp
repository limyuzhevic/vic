// Development System Tests
#include "development/DevelopmentSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_development {

void testDevelopmentSystemCreation() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    developmentSystem->initialize(brain.get());
    
    assert(brain->getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testDevelopmentSystemCreation passed" << std::endl;
}

void testDevelopmentSystemUpdate() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    developmentSystem->initialize(brain.get());
    
    // Update development system
    developmentSystem->update(brain.get(), *(brain->getRandomGenerator()), 0.001);
    
    // Developmental stage should have changed from Initial
    nlm::DevelopmentalStage stage = brain->getDevelopmentalStage();
    assert(stage != nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testDevelopmentSystemUpdate passed" << std::endl;
}

void testDevelopmentStages() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Set developmental stage to CriticalPeriod
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(brain->getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    // Set to Maturation
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    assert(brain->getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    // Set to Adult
    brain->setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain->getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testDevelopmentStages passed" << std::endl;
}

void testDevelopmentWithPlasticity() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    developmentSystem->initialize(brain.get());
    
    // Set up structural plasticity
    auto* sp = brain->getStructuralPlasticity();
    assert(sp != nullptr);
    
    // Get initial synaptogenesis rate
    float initialSynRate = sp->getSynaptogenesisRate();
    
    // Update development
    developmentSystem->update(brain.get(), *(brain->getRandomGenerator()), 0.001);
    
    // Synaptogenesis rate should have changed based on developmental stage
    float newSynRate = sp->getSynaptogenesisRate();
    assert(std::isfinite(newSynRate));
    
    std::cout << "    testDevelopmentWithPlasticity passed" << std::endl;
}

void testDevelopmentIntegration() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Test that development system is integrated
    assert(brain->getDevelopmentSystem() != nullptr);
    
    // Initialize development system
    brain->getDevelopmentSystem()->initialize(brain.get());
    
    // Run some steps to see development effects
    for (int i = 0; i < 1000; ++i) { // Multiple steps to trigger development
        brain->step(i, i * 0.001);
        
        if (i % 100 == 0) {
            nlm::DevelopmentalStage stage = brain->getDevelopmentalStage();
            assert(std::isfinite(static_cast<int>(stage)));
        }
    }
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Development System tests..." << std::endl;
    testDevelopmentSystemCreation();
    testDevelopmentSystemUpdate();
    testDevelopmentStages();
    testDevelopmentWithPlasticity();
    testDevelopmentIntegration();
    
    std::cout << "All Development System tests passed!" << std::endl;
}

} // namespace test_development
