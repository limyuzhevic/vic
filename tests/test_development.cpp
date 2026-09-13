// Development System Tests
// Tests for developmental stages and changes

#include "development/DevelopmentSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_development {

void testDevelopmentCreation() {
    nlm::DevelopmentSystem dev;
    
    // Should start in Initial stage
    assert(dev.getStage() == nlm::DevelopmentalStage::Initial);
    assert(std::string(dev.getStageName()) == "Initial");
    
    std::cout << "    testDevelopmentCreation passed" << std::endl;
}

void testDevelopmentStage() {
    nlm::DevelopmentSystem dev;
    
    // Test stage progression
    dev.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(dev.getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    assert(std::string(dev.getStageName()) == "CriticalPeriod");
    
    dev.setStage(nlm::DevelopmentalStage::Maturation);
    assert(dev.getStage() == nlm::DevelopmentalStage::Maturation);
    assert(std::string(dev.getStageName()) == "Maturation");
    
    dev.setStage(nlm::DevelopmentalStage::Adult);
    assert(dev.getStage() == nlm::DevelopmentalStage::Adult);
    assert(std::string(dev.getStageName()) == "Adult");
    
    std::cout << "    testDevelopmentStage passed" << std::endl;
}

void testDevelopmentAdvanceStage() {
    nlm::DevelopmentSystem dev;
    
    // Test automatic stage advancement
    dev.advanceStage();
    assert(dev.getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    dev.advanceStage();
    assert(dev.getStage() == nlm::DevelopmentalStage::Maturation);
    
    dev.advanceStage();
    assert(dev.getStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testDevelopmentAdvanceStage passed" << std::endl;
}

void testDevelopmentStageName() {
    nlm::DevelopmentSystem dev;
    
    assert(dev.getStageName() == std::string("Initial"));
    
    dev.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(dev.getStageName() == std::string("CriticalPeriod"));
    
    dev.setStage(nlm::DevelopmentalStage::Aging);
    assert(dev.getStageName() == std::string("Aging"));
    
    std::cout << "    testDevelopmentStageName passed" << std::endl;
}

void testDevelopmentUpdate() {
    nlm::DevelopmentSystem dev;
    
    // Create a mock brain (nullptr for testing)
    nlm::Brain* brain = nullptr;
    
    // Update should not crash
    dev.update(brain, 1000);
    
    std::cout << "    testDevelopmentUpdate passed" << std::endl;
}

void testDevelopmentUpdateWithRandom() {
    nlm::DevelopmentSystem dev;
    
    nlm::Brain* brain = nullptr;
    nlm::RandomGenerator rng(42);
    
    // Update with random should not crash
    dev.update(brain, rng, 0.1f);
    
    std::cout << "    testDevelopmentUpdateWithRandom passed" << std::endl;
}

void testDevelopmentPlasticityModifier() {
    nlm::DevelopmentSystem dev;
    
    // Initial stage should have high plasticity
    assert(dev.getPlasticityModifier() == 1.0f);
    
    // Set to different stages
    dev.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(dev.getPlasticityModifier() == 0.8f);
    
    dev.setStage(nlm::DevelopmentalStage::Maturation);
    assert(dev.getPlasticityModifier() == 0.5f);
    
    dev.setStage(nlm::DevelopmentalStage::Adult);
    assert(dev.getPlasticityModifier() == 0.2f);
    
    dev.setStage(nlm::DevelopmentalStage::Aging);
    assert(dev.getPlasticityModifier() == 0.1f);
    
    std::cout << "    testDevelopmentPlasticityModifier passed" << std::endl;
}

void testDevelopmentCriticalPeriod() {
    nlm::DevelopmentSystem dev;
    
    assert(!dev.isCriticalPeriod());
    assert(dev.getCriticalPeriodProgress() == 0.0f);
    
    dev.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(dev.isCriticalPeriod());
    
    // Progress should start at 0
    assert(dev.getCriticalPeriodProgress() == 0.0f);
    
    std::cout << "    testDevelopmentCriticalPeriod passed" << std::endl;
}

void testDevelopmentAge() {
    nlm::DevelopmentSystem dev;
    
    assert(dev.getDevelopmentalAge() == 0.0);
    
    dev.setDevelopmentalAge(100.0);
    assert(dev.getDevelopmentalAge() == 100.0);
    
    std::cout << "    testDevelopmentAge passed" << std::endl;
}

void testDevelopmentIntegration() {
    nlm::DevelopmentSystem dev;
    
    // Test integration with other systems
    // This is a placeholder for more complex integration tests
    std::cout << "    testDevelopmentIntegration passed (placeholder)" << std::endl;
}

void runAll() {
    std::cout << "Running Development tests..." << std::endl;
    testDevelopmentCreation();
    testDevelopmentStage();
    testDevelopmentAdvanceStage();
    testDevelopmentStageName();
    testDevelopmentUpdate();
    testDevelopmentUpdateWithRandom();
    testDevelopmentPlasticityModifier();
    testDevelopmentCriticalPeriod();
    testDevelopmentAge();
    testDevelopmentIntegration();
}

} // namespace test_development