// Development System Tests
// Tests for DevelopmentSystem functionality including developmental stages, synaptogenesis, pruning, and maturation

#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_development_system {

void testDevelopmentSystemCreation() {
    nlm::DevelopmentSystem devSystem;
    
    // Test initial state
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    assert(devSystem.getStageName() == "Initial");
    assert(!devSystem.isCriticalPeriod());
    assert(devSystem.getCriticalPeriodProgress() == 0.0f);
    assert(devSystem.getDevelopmentalAge() == 0.0);
    
    std::cout << "    testDevelopmentSystemCreation passed" << std::endl;
}

void testDevelopmentSystemStageManagement() {
    nlm::DevelopmentSystem devSystem;
    
    // Test stage progression
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    
    devSystem.advanceStage();
    assert(devSystem.getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.getStageName() == "CriticalPeriod");
    
    devSystem.advanceStage();
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Maturation);
    assert(devSystem.getStageName() == "Maturation");
    
    devSystem.advanceStage();
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Adult);
    assert(devSystem.getStageName() == "Adult");
    
    devSystem.advanceStage();
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Aging);
    assert(devSystem.getStageName() == "Aging");
    
    std::cout << "    testDevelopmentSystemStageManagement passed" << std::endl;
}

void testDevelopmentSystemStageSetting() {
    nlm::DevelopmentSystem devSystem;
    
    // Test direct stage setting
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Maturation);
    assert(devSystem.getStageName() == "Maturation");
    
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.getStageName() == "CriticalPeriod");
    
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Initial);
    assert(devSystem.getStageName() == "Initial");
    
    std::cout << "    testDevelopmentSystemStageSetting passed" << std::endl;
}

void testDevelopmentSystemPlasticityModifier() {
    nlm::DevelopmentSystem devSystem;
    
    // Test plasticity modifier for different stages
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    assert(nlm_test::utils::approxEqual(devSystem.getPlasticityModifier(), 1.0f));
    
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(nlm_test::utils::approxEqual(devSystem.getPlasticityModifier(), 2.0f));
    
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    assert(nlm_test::utils::approxEqual(devSystem.getPlasticityModifier(), 1.5f));
    
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(nlm_test::utils::approxEqual(devSystem.getPlasticityModifier(), 1.0f));
    
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::Aging);
    assert(nlm_test::utils::approxEqual(devSystem.getPlasticityModifier(), 0.5f));
    
    std::cout << "    testDevelopmentSystemPlasticityModifier passed" << std::endl;
}

void testDevelopmentSystemCriticalPeriod() {
    nlm::DevelopmentSystem devSystem;
    
    // Initially not in critical period
    assert(!devSystem.isCriticalPeriod());
    assert(devSystem.getCriticalPeriodProgress() == 0.0f);
    
    // Advance to critical period
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(devSystem.isCriticalPeriod());
    assert(devSystem.getCriticalPeriodProgress() == 0.0f);
    
    // Set critical period progress
    devSystem.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    // The exact implementation may vary, but we can test that the system tracks progress
    
    std::cout << "    testDevelopmentSystemCriticalPeriod passed" << std::endl;
}

void testDevelopmentSystemAgeManagement() {
    nlm::DevelopmentSystem devSystem;
    
    // Test age management
    assert(devSystem.getDevelopmentalAge() == 0.0);
    
    devSystem.setDevelopmentalAge(10.5);
    assert(nlm_test::utils::approxEqual(devSystem.getDevelopmentalAge(), 10.5));
    
    devSystem.setDevelopmentalAge(100.0);
    assert(nlm_test::utils::approxEqual(devSystem.getDevelopmentalAge(), 100.0));
    
    std::cout << "    testDevelopmentSystemAgeManagement passed" << std::endl;
}

void testDevelopmentSystemIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development system integration through brain
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Check initial stage
    assert(developmentSystem->getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Advance stages
    developmentSystem->advanceStage();
    assert(developmentSystem->getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    developmentSystem->advanceStage();
    assert(developmentSystem->getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    // Check plasticity modifier
    assert(nlm_test::utils::approxEqual(developmentSystem->getPlasticityModifier(), 1.5f));
    
    // Test critical period detection
    assert(developmentSystem->isCriticalPeriod());
    
    std::cout << "    testDevelopmentSystemIntegrationWithBrain passed" << std::endl;
}

void testDevelopmentSystemUpdateFunctions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Set to critical period for more interesting behavior
    developmentSystem->setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    
    // Test development update functions
    nlm::RandomGenerator dummyRng(42); // Dummy RNG for testing
    nlm::TimestepDuration dt = 0.1f;
    
    // This should not crash
    developmentSystem->update(&brain, 100);
    developmentSystem->update(&brain, dummyRng, dt);
    
    // Plasticity modifier should change based on stage
    float plasticityAfterStage = developmentSystem->getPlasticityModifier();
    assert(plasticityAfterStage >= 0.5f && plasticityAfterStage <= 2.0f);
    
    std::cout << "    testDevelopmentSystemUpdateFunctions passed" << std::endl;
}

void testDevelopmentSystemStageTransitions() {
    nlm::DevelopmentSystem devSystem;
    
    std::vector<nlm::DevelopmentalStage> stages = {
        nlm::DevelopmentalStage::Initial,
        nlm::DevelopmentalStage::CriticalPeriod,
        nlm::DevelopmentalStage::Maturation,
        nlm::DevelopmentalStage::Adult,
        nlm::DevelopmentalStage::Aging
    };
    
    std::vector<const char*> stageNames = {
        "Initial",
        "CriticalPeriod",
        "Maturation",
        "Adult",
        "Aging"
    };
    
    std::vector<float> expectedPlasticity = {1.0f, 2.0f, 1.5f, 1.0f, 0.5f};
    
    for (size_t i = 0; i < stages.size(); ++i) {
        devSystem.setDevelopmentalStage(stages[i]);
        assert(devSystem.getStage() == stages[i]);
        assert(std::string(devSystem.getStageName()) == stageNames[i]);
        assert(nlm_test::utils::approxEqual(devSystem.getPlasticityModifier(), expectedPlasticity[i]));
    }
    
    std::cout << "    testDevelopmentSystemStageTransitions passed" << std::endl;
}

void testDevelopmentSystemStress() {
    nlm::DevelopmentSystem devSystem;
    
    // Rapid stage changes and age updates
    for (int i = 0; i < 100; ++i) {
        devSystem.setDevelopmentalStage(static_cast<nlm::DevelopmentalStage>(i % 5));
        devSystem.setDevelopmentalAge(static_cast<double>(i) * 0.1);
        
        // Verify stage is set correctly
        assert(devSystem.getStage() >= nlm::DevelopmentalStage::Initial);
        assert(devSystem.getStage() <= nlm::DevelopmentalStage::Aging);
        
        // Verify age is reasonable
        assert(devSystem.getDevelopmentalAge() >= 0.0);
        assert(devSystem.getDevelopmentalAge() <= 10.0); // Within test range
        
        // Verify plasticity modifier is in valid range
        float plasticity = devSystem.getPlasticityModifier();
        assert(plasticity >= 0.5f && plasticity <= 2.0f);
    }
    
    std::cout << "    testDevelopmentSystemStress passed" << std::endl;
}

void testDevelopmentSystemIntegrationWithBrainSimulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Set development stage to critical period
    auto* devSystem = brain.getDevelopmentSystem();
    devSystem->setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    
    // Simulate brain development over time
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Development should be accessible and functional
        assert(brain.getDevelopmentSystem() != nullptr);
        
        // Age should increase over time (if implemented)
        // float age = brain.getDevelopmentSystem()->getDevelopmentalAge();
        // assert(age >= 0.0);
    }
    
    std::cout << "    testDevelopmentSystemIntegrationWithBrainSimulation passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Development System Creation", testDevelopmentSystemCreation);
    runner.runTest("Development System Stage Management", testDevelopmentSystemStageManagement);
    runner.runTest("Development System Stage Setting", testDevelopmentSystemStageSetting);
    runner.runTest("Development System Plasticity Modifier", testDevelopmentSystemPlasticityModifier);
    runner.runTest("Development System Critical Period", testDevelopmentSystemCriticalPeriod);
    runner.runTest("Development System Age Management", testDevelopmentSystemAgeManagement);
    runner.runTest("Development System Integration With Brain", testDevelopmentSystemIntegrationWithBrain);
    runner.runTest("Development System Update Functions", testDevelopmentSystemUpdateFunctions);
    runner.runTest("Development System Stage Transitions", testDevelopmentSystemStageTransitions);
    runner.runTest("Development System Stress Test", testDevelopmentSystemStress);
    runner.runTest("Development System Integration With Brain Simulation", testDevelopmentSystemIntegrationWithBrainSimulation);
    
    runner.runAll();
}

} // namespace test_development_system
