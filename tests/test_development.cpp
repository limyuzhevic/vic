// Development System Tests
#include "development/DevelopmentSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_development {

void testDevelopmentSystemCreation() {
    nlm::DevelopmentSystem dev;
    
    assert(dev.getStage() == nlm::DevelopmentalStage::Initial);
    assert(dev.getDevelopmentalAge() == 0.0);
    assert(dev.getPlasticityModifier() == 1.0f);
    assert(dev.getStageName() == std::string("Initial"));
    
    std::cout << "    testDevelopmentSystemCreation passed" << std::endl;
}

void testDevelopmentSystemInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    
    nlm::DevelopmentSystem dev;
    
    dev.initialize(brain, nullptr, 0.001);
    
    assert(dev.getBrain() != nullptr);
    
    std::cout << "    testDevelopmentSystemInitialization passed" << std::endl;
}

void testDevelopmentalStageProgression() {
    nlm::DevelopmentSystem dev;
    
    // Initial stage
    assert(dev.getStage() == nlm::DevelopmentalStage::Initial);
    assert(dev.getPlasticityModifier() == 1.0f);
    
    // Advance stage
    dev.advanceStage();
    assert(dev.getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    assert(dev.getPlasticityModifier() == 0.8f);
    
    // Advance more
    dev.advanceStage();
    assert(dev.getStage() == nlm::DevelopmentalStage::Maturation);
    assert(dev.getPlasticityModifier() == 0.5f);
    
    // Advance to adulthood
    dev.advanceStage();
    assert(dev.getStage() == nlm::DevelopmentalStage::Adult);
    assert(dev.getPlasticityModifier() == 0.2f);
    
    // Aging stage
    dev.advanceStage();
    assert(dev.getStage() == nlm::DevelopmentalStage::Aging);
    assert(dev.getPlasticityModifier() == 0.1f);
    
    std::cout << "    testDevelopmentalStageProgression passed" << std::endl;
}

void testDevelopmentSystemUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    
    nlm::DevelopmentSystem dev;
    
    // Update with step-based progression
    for (int step = 0; step < 70; ++step) {
        dev.update(brain, static_cast<nlm::SimulationStep>(step), 0.001);
        
        // Stage should advance around step 60
        if (step >= 60 && dev.getStage() != nlm::DevelopmentalStage::CriticalPeriod) {
            assert(dev.getStage() == nlm::DevelopmentalStage::CriticalPeriod ||
                   dev.getStage() == nlm::DevelopmentalStage::Maturation);
        }
    }
    
    std::cout << "    testDevelopmentSystemUpdate passed" << std::endl;
}

void testDevelopmentSystemAgeBasedProgression() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    
    nlm::DevelopmentSystem dev;
    
    // Update with age-based progression
    dev.update(brain, nullptr, 10.0);  // 10 seconds
    assert(dev.getDevelopmentalAge() >= 10.0);
    
    dev.update(brain, nullptr, 60.0);  // Another 60 seconds - should advance through stages
    // Age should be around 70, past Initial stage (60s) but not yet past CriticalPeriod (360s)
    assert(dev.getDevelopmentalAge() >= 70.0);
    
    std::cout << "    testDevelopmentSystemAgeBasedProgression passed" << std::endl;
}

void testPlasticityModulator() {
    nlm::DevelopmentSystem dev;
    
    // Plasticity modifier should vary by stage
    assert(dev.getStage() == nlm::DevelopmentalStage::Initial);
    assert(dev.getPlasticityModifier() == 1.0f);
    
    dev.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(dev.getPlasticityModifier() == 0.8f);
    
    dev.setStage(nlm::DevelopmentalStage::Maturation);
    assert(dev.getPlasticityModifier() == 0.5f);
    
    dev.setStage(nlm::DevelopmentalStage::Adult);
    assert(dev.getPlasticityModifier() == 0.2f);
    
    dev.setStage(nlm::DevelopmentalStage::Aging);
    assert(dev.getPlasticityModifier() == 0.1f);
    
    std::cout << "    testPlasticityModulator passed" << std::endl;
}

void testCriticalPeriod() {
    nlm::DevelopmentSystem dev;
    
    // Initially not in critical period
    assert(!dev.isCriticalPeriod());
    assert(dev.getCriticalPeriodProgress() == 0.0f);
    
    // Advance to critical period
    dev.setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(dev.isCriticalPeriod());
    
    // Progress through critical period
    dev.setDevelopmentalAge(150.0);  // 150 out of 300
    assert(dev.getCriticalPeriodProgress() >= 0.49f && dev.getCriticalPeriodProgress() <= 0.51f);
    
    dev.setDevelopmentalAge(300.0);  // Complete critical period
    assert(dev.getCriticalPeriodProgress() >= 0.99f);
    
    std::cout << "    testCriticalPeriod passed" << std::endl;
}

void testDevelopmentSystemReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    
    nlm::DevelopmentSystem dev;
    
    // Modify state
    dev.advanceStage();
    dev.setDevelopmentalAge(50.0);
    
    // Reset by creating new instance (since DevelopmentSystem doesn't have explicit reset)
    nlm::DevelopmentSystem dev2;
    
    assert(dev2.getStage() == nlm::DevelopmentalStage::Initial);
    assert(dev2.getDevelopmentalAge() == 0.0);
    
    std::cout << "    testDevelopmentSystemReset passed" << std::endl;
}

void testIntegratedDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    
    nlm::DevelopmentSystem dev;
    
    // Simulate brain development over time
    for (double age = 0.0; age < 1000.0; age += 10.0) {
        dev.update(brain, nullptr, age);
        
        // Check that stage is valid
        assert(dev.getStage() != nlm::DevelopmentalStage::Aging || age >= 900.0);
        
        // Plasticity modifier should decrease with age
        float expectedMaxPlasticity = 1.0f;
        if (age >= 60.0) expectedMaxPlasticity = 0.8f;
        if (age >= 360.0) expectedMaxPlasticity = 0.5f;
        if (age >= 960.0) expectedMaxPlasticity = 0.2f;
        
        assert(dev.getPlasticityModifier() <= expectedMaxPlasticity + 0.01f);
    }
    
    std::cout << "    testIntegratedDevelopment passed" << std::endl;
}

void runAll() {
    std::cout << "Running Development system tests..." << std::endl;
    
    testDevelopmentSystemCreation();
    testDevelopmentSystemInitialization();
    testDevelopmentalStageProgression();
    testDevelopmentSystemUpdate();
    testDevelopmentSystemAgeBasedProgression();
    testPlasticityModulator();
    testCriticalPeriod();
    testDevelopmentSystemReset();
    testIntegratedDevelopment();
}

} // namespace test_development
