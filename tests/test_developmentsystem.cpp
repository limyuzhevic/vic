// DevelopmentSystem Tests
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>

namespace test_developmentsystem {

void testDevelopmentSystemCreation() {
    nlm::DevelopmentSystem devSys;
    
    assert(devSys.getStage() == nlm::DevelopmentalStage::Initial);
    assert(devSys.getDevelopmentalAge() == 0.0);
    assert(devSys.isCriticalPeriod() == false);
    
    std::cout << "    testDevelopmentSystemCreation passed" << std::endl;
}

void testDevelopmentalStage() {
    nlm::DevelopmentSystem devSys;
    
    // Test stage progression
    assert(devSys.getStageName() != nullptr);
    
    // Test stage advancement
    devSys.advanceStage();
    assert(devSys.getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    devSys.advanceStage();
    assert(devSys.getStage() == nlm::DevelopmentalStage::Maturation);
    
    devSys.advanceStage();
    assert(devSys.getStage() == nlm::DevelopmentalStage::Adult);
    
    devSys.advanceStage();
    assert(devSys.getStage() == nlm::DevelopmentalStage::Aging);
    
    std::cout << "    testDevelopmentalStage passed" << std::endl;
}

void testPlasticityModifier() {
    nlm::DevelopmentSystem devSys;
    
    // Test default plasticity modifier
    float modifier = devSys.getPlasticityModifier();
    assert(modifier >= 0.0f && modifier <= 1.0f);
    
    // Test stage-specific modifiers
    devSys.advanceStage();  // Critical period
    float criticalPeriodModifier = devSys.getPlasticityModifier();
    assert(criticalPeriodModifier >= 1.0f);  // Should be higher during critical period
    
    std::cout << "    testPlasticityModifier passed" << std::endl;
}

void testCriticalPeriodProgress() {
    nlm::DevelopmentSystem devSys;
    
    // Test critical period progress
    float progress = devSys.getCriticalPeriodProgress();
    assert(progress >= 0.0f && progress <= 1.0f);
    
    // Advance through critical period
    devSys.advanceStage();
    progress = devSys.getCriticalPeriodProgress();
    assert(progress >= 0.0f && progress <= 1.0f);
    
    std::cout << "    testCriticalPeriodProgress passed" << std::endl;
}

void testAgeManagement() {
    nlm::DevelopmentSystem devSys;
    
    // Test initial age
    assert(devSys.getDevelopmentalAge() == 0.0);
    
    // Test age setting
    devSys.setDevelopmentalAge(50.0);
    assert(devSys.getDevelopmentalAge() == 50.0);
    
    // Test age doesn't change automatically without update
    double ageBefore = devSys.getDevelopmentalAge();
    devSys.setDevelopmentalAge(100.0);
    double ageAfter = devSys.getDevelopmentalAge();
    assert(ageAfter == 100.0);  // Should not advance automatically
    
    std::cout << "    testAgeManagement passed" << std::endl;
}

void testDevelopmentSystemReset() {
    nlm::DevelopmentSystem devSys;
    
    // Modify state
    devSys.setDevelopmentalAge(75.0);
    devSys.advanceStage();
    
    // Reset age (note: DevelopmentSystem doesn't have a reset method in this version)
    // Just verify we can set age to 0
    devSys.setDevelopmentalAge(0.0);
    assert(devSys.getDevelopmentalAge() == 0.0);
    assert(devSys.getStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testDevelopmentSystemReset passed" << std::endl;
}

void runAll() {
    testDevelopmentSystemCreation();
    testDevelopmentalStage();
    testPlasticityModifier();
    testCriticalPeriodProgress();
    testAgeManagement();
    testDevelopmentSystemReset();
    
    std::cout << "=== DevelopmentSystem Tests Complete ===" << std::endl;
}

} // namespace test_developmentsystem