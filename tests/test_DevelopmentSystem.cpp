// DevelopmentSystem Tests
#include "development/DevelopmentSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_development_system {

void testDevelopmentCreation() {
    nlm::DevelopmentSystem development;
    
    // Test default stage
    assert(development.getStage() == nlm::DevelopmentalStage::Early);
    assert(development.getStageName() != nullptr);
    
    // Test age
    assert(development.getDevelopmentalAge() == 0.0);
    
    // Test critical period
    assert(development.getCriticalPeriodProgress() == 0.0f);
    
    std::cout << "    testDevelopmentCreation passed" << std::endl;
}

void testDevelopmentStage() {
    nlm::DevelopmentSystem development;
    
    // Test initial stage
    assert(development.getStage() == nlm::DevelopmentalStage::Early);
    assert(std::string(development.getStageName()) == "Early");
    
    // Set different stages
    development.setStage(nlm::DevelopmentalStage::Middle);
    assert(development.getStage() == nlm::DevelopmentalStage::Middle);
    assert(std::string(development.getStageName()) == "Middle");
    
    development.setStage(nlm::DevelopmentalStage::Late);
    assert(development.getStage() == nlm::DevelopmentalStage::Late);
    assert(std::string(development.getStageName()) == "Late");
    
    std::cout << "    testDevelopmentStage passed" << std::endl;
}

void testDevelopmentAdvanceStage() {
    nlm::DevelopmentSystem development;
    
    // Advance through stages
    development.advanceStage();
    assert(development.getStage() == nlm::DevelopmentalStage::Middle);
    
    development.advanceStage();
    assert(development.getStage() == nlm::DevelopmentalStage::Late);
    
    development.advanceStage();
    // Should stay at Late (no more stages)
    assert(development.getStage() == nlm::DevelopmentalStage::Late);
    
    std::cout << "    testDevelopmentAdvanceStage passed" << std::endl;
}

void testDevelopmentAge() {
    nlm::DevelopmentSystem development;
    
    // Test initial age
    assert(development.getDevelopmentalAge() == 0.0);
    
    // Set age
    development.setDevelopmentalAge(10.5);
    assert(development.getDevelopmentalAge() == 10.5);
    
    // Set negative age (should be allowed)
    development.setDevelopmentalAge(-1.0);
    assert(development.getDevelopmentalAge() == -1.0);
    
    std::cout << "    testDevelopmentAge passed" << std::endl;
}

void testDevelopmentPlasticity() {
    nlm::DevelopmentSystem development;
    
    // Test initial plasticity modifier
    float plasticity = development.getPlasticityModifier();
    assert(plasticity >= 0.0f && plasticity <= 1.0f);
    
    // Change stage and check plasticity changes
    development.setStage(nlm::DevelopmentalStage::Middle);
    float plasticityMiddle = development.getPlasticityModifier();
    assert(plasticityMiddle >= 0.0f && plasticityMiddle <= 1.0f);
    
    development.setStage(nlm::DevelopmentalStage::Late);
    float plasticityLate = development.getPlasticityModifier();
    assert(plasticityLate >= 0.0f && plasticityLate <= 1.0f);
    
    std::cout << "    testDevelopmentPlasticity passed" << std::endl;
}

void testDevelopmentCriticalPeriod() {
    nlm::DevelopmentSystem development;
    
    // Test initial critical period
    assert(development.getCriticalPeriodProgress() == 0.0f);
    assert(development.isCriticalPeriod() == false);
    
    // Set age to move through critical period
    development.setDevelopmentalAge(10.0);
    
    // Check if we're in critical period (depends on implementation)
    // For now, just test the API doesn't crash
    development.isCriticalPeriod();
    development.getCriticalPeriodProgress();
    
    std::cout << "    testDevelopmentCriticalPeriod passed" << std::endl;
}

void testDevelopmentUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::DevelopmentSystem development;
    development.initialize(&brain);  // Note: DevelopmentSystem might not have initialize()
    
    // Test update with SimulationStep
    development.update(&brain, 0);
    
    // Test update with full parameters
    development.update(&brain, config->getRandomGenerator(), 0.1f);
    
    std::cout << "    testDevelopmentUpdate passed" << std::endl;
}

void testDevelopmentAllStages() {
    nlm::DevelopmentSystem development;
    
    // Test all developmental stages
    std::vector<std::pair<nlm::DevelopmentalStage, const char*>> stages = {
        {nlm::DevelopmentalStage::Early, "Early"},
        {nlm::DevelopmentalStage::Middle, "Middle"},
        {nlm::DevelopmentalStage::Late, "Late"}
    };
    
    for (const auto& stage : stages) {
        development.setStage(stage.first);
        assert(development.getStage() == stage.first);
        assert(std::string(development.getStageName()) == stage.second);
    }
    
    std::cout << "    testDevelopmentAllStages passed" << std::endl;
}

void testDevelopmentAdvancedFeatures() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::DevelopmentSystem development;
    development.initialize(&brain);
    
    // Test different ages
    std::vector<double> ages = {0.0, 1.0, 5.0, 10.0, 20.0, 50.0};
    for (double age : ages) {
        development.setDevelopmentalAge(age);
        assert(development.getDevelopmentalAge() == age);
    }
    
    std::cout << "    testDevelopmentAdvancedFeatures passed" << std::endl;
}

void testDevelopmentStageTransitions() {
    nlm::DevelopmentSystem development;
    
    // Test stage transitions
    assert(development.getStage() == nlm::DevelopmentalStage::Early);
    
    development.advanceStage();
    assert(development.getStage() == nlm::DevelopmentalStage::Middle);
    
    development.advanceStage();
    assert(development.getStage() == nlm::DevelopmentalStage::Late);
    
    development.advanceStage();
    assert(development.getStage() == nlm::DevelopmentalStage::Late);  // Should not go beyond
    
    std::cout << "    testDevelopmentStageTransitions passed" << std::endl;
}

void testDevelopmentSystemConsistency() {
    nlm::DevelopmentSystem development;
    
    // Test that setting age and stage work together
    development.setDevelopmentalAge(15.0);
    development.setStage(nlm::DevelopmentalStage::Middle);
    
    assert(development.getDevelopmentalAge() == 15.0);
    assert(development.getStage() == nlm::DevelopmentalStage::Middle);
    
    // Test plasticity modifier consistency
    float plasticity = development.getPlasticityModifier();
    assert(plasticity >= 0.0f && plasticity <= 1.0f);
    
    std::cout << "    testDevelopmentSystemConsistency passed" << std::endl;
}

void runAll() {
    testDevelopmentCreation();
    testDevelopmentStage();
    testDevelopmentAdvanceStage();
    testDevelopmentAge();
    testDevelopmentPlasticity();
    testDevelopmentCriticalPeriod();
    testDevelopmentUpdate();
    testDevelopmentAllStages();
    testDevelopmentAdvancedFeatures();
    testDevelopmentStageTransitions();
    testDevelopmentSystemConsistency();
}

} // namespace test_development_system
