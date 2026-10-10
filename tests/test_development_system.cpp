// DevelopmentSystem Tests
#include "development/DevelopmentSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>

namespace test_development_system {

void testDevelopmentSystemCreation() {
    nlm::DevelopmentSystem devSystem;
    
    // Default stage should be Initial
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    assert(std::string(devSystem.getStageName()) == "Initial");
    
    // Default plasticity modifier
    assert(devSystem.getPlasticityModifier() == 1.0f);
    
    // Not critical period by default
    assert(devSystem.isCriticalPeriod() == false);
    assert(devSystem.getCriticalPeriodProgress() == 0.0f);
    
    // Default developmental age
    assert(devSystem.getDevelopmentalAge() == 0.0);
    
    std::cout << "    testDevelopmentSystemCreation passed" << std::endl;
}

void testDevelopmentSystemStageManagement() {
    nlm::DevelopmentSystem devSystem;
    
    // Set stage to CriticalPeriod
    devSystem.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    assert(std::string(devSystem.getStageName()) == "CriticalPeriod");
    
    // Advance stage
    devSystem.advanceStage();
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Maturation);
    
    // Set back to Initial
    devSystem.setStage(nlm::DevelopmentalStage::Initial);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    assert(std::string(devSystem.getStageName()) == "Initial");
    
    std::cout << "    testDevelopmentSystemStageManagement passed" << std::endl;
}

void testDevelopmentSystemPlasticityModifier() {
    nlm::DevelopmentSystem devSystem;
    
    // Test plasticity modifier for each stage
    devSystem.setStage(nlm::DevelopmentalStage::Initial);
    assert(devSystem.getPlasticityModifier() == 1.0f);
    
    devSystem.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.getPlasticityModifier() == 0.8f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Maturation);
    assert(devSystem.getPlasticityModifier() == 0.5f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Adult);
    assert(devSystem.getPlasticityModifier() == 0.2f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Aging);
    assert(devSystem.getPlasticityModifier() == 0.1f);
    
    std::cout << "    testDevelopmentSystemPlasticityModifier passed" << std::endl;
}

void testDevelopmentSystemCriticalPeriod() {
    nlm::DevelopmentSystem devSystem;
    
    // Initially not in critical period
    assert(devSystem.isCriticalPeriod() == false);
    assert(devSystem.getCriticalPeriodProgress() == 0.0f);
    
    // Set to critical period
    devSystem.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.isCriticalPeriod() == true);
    assert(devSystem.getCriticalPeriodProgress() == 0.0f);
    
    std::cout << "    testDevelopmentSystemCriticalPeriod passed" << std::endl;
}

void testDevelopmentSystemAgeManagement() {
    nlm::DevelopmentSystem devSystem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Update with simple world
    nlm::SimpleWorld world;
    
    // Initial age
    assert(devSystem.getDevelopmentalAge() == 0.0);
    
    // Set age
    devSystem.setDevelopmentalAge(100.0);
    assert(devSystem.getDevelopmentalAge() == 100.0);
    
    std::cout << "    testDevelopmentSystemAgeManagement passed" << std::endl;
}

void testDevelopmentSystemUpdate() {
    nlm::DevelopmentSystem devSystem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    nlm::SimpleWorld world;
    
    // Update development
    devSystem.update(&brain, 0);
    
    // Age should have increased
    assert(devSystem.getDevelopmentalAge() > 0.0);
    
    // Test with different parameters
    devSystem.update(&brain, world, 1.0f);
    
    // Should still work
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testDevelopmentSystemUpdate passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    nlm::DevelopmentSystem devSystem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::SimpleWorld world;
    
    // Test integration with brain
    devSystem.update(&brain, 0);
    
    // Test integration with random generator
    nlm::RandomGenerator rng(42);
    devSystem.update(&brain, rng, 0.1f);
    
    // Stage name should still be valid
    assert(std::string(devSystem.getStageName()) != "");
    
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running DevelopmentSystem tests..." << std::endl;
    testDevelopmentSystemCreation();
    testDevelopmentSystemStageManagement();
    testDevelopmentSystemPlasticityModifier();
    testDevelopmentSystemCriticalPeriod();
    testDevelopmentSystemAgeManagement();
    testDevelopmentSystemUpdate();
    testDevelopmentSystemIntegration();
}

} // namespace test_development_system
