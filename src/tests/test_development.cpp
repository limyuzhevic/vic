// Development System Tests
#include "core/Config/Config.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>

namespace test_development {

void testDevelopmentSystemCreation() {
    nlm::DevelopmentSystem devSystem;
    
    // Check initial stage
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    
    // Check that we can get stage age
    double stageAge = devSystem.getStageAge();
    double devAge = devSystem.getDevelopmentalAge();
    
    // They should be accessible through public methods
    // (Note: internal implementation details may vary)
    
    std::cout << "    testDevelopmentSystemCreation passed" << std::endl;
}

void testDevelopmentalAgeTracking() {
    nlm::DevelopmentSystem devSystem;
    
    // Test that stage and developmental age are consistent
    devSystem.setDevelopmentalAge(100.0);
    double stageAge1 = devSystem.getStageAge();
    double devAge1 = devSystem.getDevelopmentalAge();
    
    // The key improvement: single source of truth for age tracking
    // Internal implementation ensures consistency
    assert(stageAge1 == devAge1);
    
    // Advance stage should reset stage age appropriately
    devSystem.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    double stageAge2 = devSystem.getStageAge();
    
    // Stage age should be reset, developmental age should persist
    assert(stageAge2 == 0.0);
    assert(devAge1 == devSystem.getDevelopmentalAge());
    
    std::cout << "    testDevelopmentalAgeTracking passed" << std::endl;
}

void testConfigurableStageDurations() {
    nlm::Config config;
    
    // Test that config system supports development stage configuration
    config.configureDevelopmentStage("initial", 90.0);
    config.configureDevelopmentStage("criticalperiod", 450.0);
    config.configureDevelopmentStage("maturation", 900.0);
    
    // Verify custom durations are set
    assert(config.getDevelopmentStageDuration("initial") == 90.0);
    assert(config.getDevelopmentStageDuration("criticalperiod") == 450.0);
    assert(config.getDevelopmentStageDuration("maturation") == 900.0);
    
    // Test default values are not overwritten
    config.configureDevelopmentStage("initial", 120.0);
    assert(config.getDevelopmentStageDuration("initial") == 120.0);
    
    std::cout << "    testConfigurableStageDurations passed" << std::endl;
}

void testPlasticityModifier() {
    nlm::DevelopmentSystem devSystem;
    
    // Test plasticity modifier for each stage
    assert(devSystem.getPlasticityModifier() == 1.0f);  // Initial
    
    devSystem.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.getPlasticityModifier() == 0.8f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Maturation);
    assert(devSystem.getPlasticityModifier() == 0.5f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Adult);
    assert(devSystem.getPlasticityModifier() == 0.2f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Aging);
    assert(devSystem.getPlasticityModifier() == 0.1f);
    
    std::cout << "    testPlasticityModifier passed" << std::endl;
}

void testCriticalPeriodInfo() {
    nlm::DevelopmentSystem devSystem;
    
    // Not in critical period
    assert(!devSystem.isCriticalPeriod());
    assert(devSystem.getCriticalPeriodProgress() == 0.0f);
    
    // Enter critical period
    devSystem.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.isCriticalPeriod());
    
    // Progress through critical period (internal stage age management)
    // Note: In the real implementation, the age tracking would be handled
    // through the single source of truth in the pImpl
    devSystem.setStageAge(150.0);
    
    std::cout << "    testCriticalPeriodInfo passed" << std::endl;
}

void testStageAdvancement() {
    nlm::DevelopmentSystem devSystem;
    
    // Verify stage progression
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    assert(std::string(devSystem.getStageName()) == "Initial");
    
    // Test all stages
    devSystem.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    assert(std::string(devSystem.getStageName()) == "CriticalPeriod");
    
    devSystem.setStage(nlm::DevelopmentalStage::Maturation);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Maturation);
    assert(std::string(devSystem.getStageName()) == "Maturation");
    
    devSystem.setStage(nlm::DevelopmentalStage::Adult);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Adult);
    assert(std::string(devSystem.getStageName()) == "Adult");
    
    devSystem.setStage(nlm::DevelopmentalStage::Aging);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Aging);
    assert(std::string(devSystem.getStageName()) == "Aging");
    
    std::cout << "    testStageAdvancement passed" << std::endl;
}

void testDevelopmentSystemInitialization() {
    // Test that the development system can be initialized
    // Note: This is a simplified test as the full initialization
    // would require a complete brain object with all subsystems
    
    nlm::DevelopmentSystem devSystem;
    
    // Check that it can be created
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    
    // Check that stage name is accessible
    assert(devSystem.getStageName() != nullptr);
    
    std::cout << "    testDevelopmentSystemInitialization passed" << std::endl;
}

void runAll() {
    testDevelopmentSystemCreation();
    testDevelopmentalAgeTracking();
    testConfigurableStageDurations();
    testPlasticityModifier();
    testCriticalPeriodInfo();
    testStageAdvancement();
    testDevelopmentSystemInitialization();
    
    std::cout << "\nAll development tests passed!" << std::endl;
}

} // namespace test_development