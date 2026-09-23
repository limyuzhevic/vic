// DevelopmentSystem Tests
#include "brain/DevelopmentSystem.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_developmentsystem {

void testDevelopmentSystemCreation() {
    nlm::DevelopmentSystem devsys;
    
    assert(devsys.getStage() == nlm::DevelopmentalStage::EARLY);
    assert(std::string(devsys.getStageName()) == "EARLY");
    
    std::cout << "    testDevelopmentSystemCreation passed" << std::endl;
}

void testDevelopmentSystemStageAdvancement() {
    nlm::DevelopmentSystem devsys;
    
    // Advance through stages
    devsys.advanceStage();
    assert(devsys.getStage() == nlm::DevelopmentalStage::MIDDLE);
    assert(std::string(devsys.getStageName()) == "MIDDLE");
    
    devsys.advanceStage();
    assert(devsys.getStage() == nlm::DevelopmentalStage::LATE);
    assert(std::string(devsys.getStageName()) == "LATE");
    
    devsys.advanceStage();
    assert(devsys.getStage() == nlm::DevelopmentalStage::MATURATION);
    assert(std::string(devsys.getStageName()) == "MATURATION");
    
    devsys.advanceStage();
    assert(devsys.getStage() == nlm::DevelopmentalStage::ADULT);
    assert(std::string(devsys.getStageName()) == "ADULT");
    
    std::cout << "    testDevelopmentSystemStageAdvancement passed" << std::endl;
}

void testDevelopmentSystemStageSetting() {
    nlm::DevelopmentSystem devsys;
    
    devsys.setStage(nlm::DevelopmentalStage::MIDDLE);
    assert(devsys.getStage() == nlm::DevelopmentalStage::MIDDLE);
    assert(std::string(devsys.getStageName()) == "MIDDLE");
    
    devsys.setStage(nlm::DevelopmentalStage::LATE);
    assert(devsys.getStage() == nlm::DevelopmentalStage::LATE);
    
    std::cout << "    testDevelopmentSystemStageSetting passed" << std::endl;
}

void testDevelopmentSystemPlasticityModifier() {
    nlm::DevelopmentSystem devsys;
    
    // Default stage should have some plasticity
    float modifier = devsys.getPlasticityModifier();
    assert(modifier >= 0.0f && modifier <= 1.0f);
    
    // Stage affects plasticity
    devsys.advanceStage();
    float modifierAfter = devsys.getPlasticityModifier();
    assert(modifierAfter >= 0.0f && modifierAfter <= 1.0f);
    
    std::cout << "    testDevelopmentSystemPlasticityModifier passed" << std::endl;
}

void testDevelopmentSystemCriticalPeriod() {
    nlm::DevelopmentSystem devsys;
    
    // Initially not in critical period
    assert(!devsys.isCriticalPeriod());
    
    float progress = devsys.getCriticalPeriodProgress();
    assert(progress >= 0.0f && progress <= 1.0f);
    
    std::cout << "    testDevelopmentSystemCriticalPeriod passed" << std::endl;
}

void testDevelopmentSystemDevelopmentalAge() {
    nlm::DevelopmentSystem devsys;
    
    assert(devsys.getDevelopmentalAge() == 0.0);
    
    devsys.setDevelopmentalAge(100.0);
    assert(devsys.getDevelopmentalAge() == 100.0);
    
    std::cout << "    testDevelopmentSystemDevelopmentalAge passed" << std::endl;
}

void testDevelopmentSystemUpdateWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::DevelopmentSystem devsys;
    
    // Test simple update
    devsys.update(&brain, 0);
    
    // Test update with rng and dt
    nlm::RandomGenerator rng(42);
    devsys.update(&brain, rng, 0.001);
    
    std::cout << "    testDevelopmentSystemUpdateWithBrain passed" << std::endl;
}

void runAll() {
    std::cout << "Running DevelopmentSystem tests..." << std::endl;
    testDevelopmentSystemCreation();
    testDevelopmentSystemStageAdvancement();
    testDevelopmentSystemStageSetting();
    testDevelopmentSystemPlasticityModifier();
    testDevelopmentSystemCriticalPeriod();
    testDevelopmentSystemDevelopmentalAge();
    testDevelopmentSystemUpdateWithBrain();
}

} // namespace test_developmentsystem