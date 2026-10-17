// Development System Tests
#include "development/DevelopmentSystem.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_development {

void testDevelopmentSystem() {
    nlm::DevelopmentSystem developmentSystem;
    
    // Test initial state
    assert(developmentSystem.getStage() == nlm::DevelopmentalStage::Initial);
    assert(developmentSystem.getStageName() == std::string("Initial"));
    
    // Test plasticity modifier for initial stage
    float plasticity = developmentSystem.getPlasticityModifier();
    assert(plasticity == 1.0f);
    
    // Test critical period check
    assert(developmentSystem.isCriticalPeriod() == false);
    assert(developmentSystem.getCriticalPeriodProgress() == 0.0f);
    
    // Test advancement
    developmentSystem.advanceStage();
    assert(developmentSystem.getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    assert(developmentSystem.getStageName() == std::string("CriticalPeriod"));
    
    // Test plasticity for critical period
    plasticity = developmentSystem.getPlasticityModifier();
    assert(plasticity == 0.8f);
    
    // Test critical period check after advancement
    assert(developmentSystem.isCriticalPeriod() == true);
    
    // Test further advancement
    developmentSystem.advanceStage();
    assert(developmentSystem.getStage() == nlm::DevelopmentalStage::Maturation);
    assert(developmentSystem.getStageName() == std::string("Maturation"));
    
    // Test plasticity for maturation
    plasticity = developmentSystem.getPlasticityModifier();
    assert(plasticity == 0.5f);
    
    // Test further advancement
    developmentSystem.advanceStage();
    assert(developmentSystem.getStage() == nlm::DevelopmentalStage::Adult);
    assert(developmentSystem.getStageName() == std::string("Adult"));
    
    // Test plasticity for adult
    plasticity = developmentSystem.getPlasticityModifier();
    assert(plasticity == 0.2f);
    
    // Test aging stage
    developmentSystem.advanceStage();
    assert(developmentSystem.getStage() == nlm::DevelopmentalStage::Aging);
    assert(developmentSystem.getStageName() == std::string("Aging"));
    
    // Test plasticity for aging
    plasticity = developmentSystem.getPlasticityModifier();
    assert(plasticity == 0.0f);
    
    // Test age
    double age = developmentSystem.getDevelopmentalAge();
    assert(age == 0.0);
    
    // Test age setting
    developmentSystem.setDevelopmentalAge(100.0);
    assert(developmentSystem.getDevelopmentalAge() == 100.0);
    
    // Test manual stage setting
    developmentSystem.setStage(nlm::DevelopmentalStage::Initial);
    assert(developmentSystem.getStage() == nlm::DevelopmentalStage::Initial);
    
    // Test update functions
    nlm::RandomGenerator rng(42);
    developmentSystem.update(nullptr, rng, 0.1f);
    developmentSystem.update(nullptr, 0); // Simple update
    
    std::cout << "    testDevelopmentSystem passed" << std::endl;
}

void testDevelopmentPlasticityStages() {
    nlm::DevelopmentSystem developmentSystem;
    
    // Test plasticity modifier progression through stages
    float initialPlasticity = developmentSystem.getPlasticityModifier();
    assert(initialPlasticity == 1.0f);
    
    developmentSystem.advanceStage();
    float criticalPeriodPlasticity = developmentSystem.getPlasticityModifier();
    assert(criticalPeriodPlasticity == 0.8f);
    
    developmentSystem.advanceStage();
    float maturationPlasticity = developmentSystem.getPlasticityModifier();
    assert(maturationPlasticity == 0.5f);
    
    developmentSystem.advanceStage();
    float adultPlasticity = developmentSystem.getPlasticityModifier();
    assert(adultPlasticity == 0.2f);
    
    developmentSystem.advanceStage();
    float agingPlasticity = developmentSystem.getPlasticityModifier();
    assert(agingPlasticity == 0.0f);
    
    // Test critical period progress
    assert(developmentSystem.getCriticalPeriodProgress() == 0.0f);
    
    std::cout << "    testDevelopmentPlasticityStages passed" << std::endl;
}

void testDevelopmentAgeTracking() {
    nlm::DevelopmentSystem developmentSystem;
    
    // Test age tracking
    double initialAge = developmentSystem.getDevelopmentalAge();
    assert(initialAge == 0.0);
    
    developmentSystem.setDevelopmentalAge(50.0);
    assert(developmentSystem.getDevelopmentalAge() == 50.0);
    
    // Test update increases age
    nlm::RandomGenerator rng(42);
    developmentSystem.update(nullptr, rng, 1.0f);
    assert(developmentSystem.getDevelopmentalAge() > 50.0);
    
    // Test stage age tracking
    developmentSystem.advanceStage();
    // After advancing, stage age should reset
    
    std::cout << "    testDevelopmentAgeTracking passed" << std::endl;
}

void runAll() {
    testDevelopmentSystem();
    testDevelopmentPlasticityStages();
    testDevelopmentAgeTracking();
}

} // namespace test_development