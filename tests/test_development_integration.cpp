// Development System Integration Tests
// Tests the comprehensive integration of the DevelopmentSystem across all brain systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_development_integration {

void testDevelopmentSystemIntegration() {
    std::cout << "=== Testing Development System Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    bool success = brain.initialize();
    assert(success);
    
    // Get development system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    std::cout << "Initial developmental stage: " << developmentSystem->getStageName() << std::endl;
    std::cout << "Initial developmental age: " << developmentSystem->getDevelopmentalAge() << std::endl;
    
    // Test initial stage effects
    assert(developmentSystem->getStage() == nlm::DevelopmentalStage::Initial);
    assert(developmentSystem->getPlasticityModifier() == 1.0f);
    
    // Test system integration
    // This should not crash and should apply developmental effects
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    std::cout << "After 100 steps, developmental stage: " << developmentSystem->getStageName() << std::endl;
    std::cout << "After 100 steps, developmental age: " << developmentSystem->getDevelopmentalAge() << std::endl;
    
    // Test critical period detection
    // In real implementation, this would be tested when in CriticalPeriod stage
    
    // Test system access
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getAcetylcholine() != nullptr);
    assert(brain.getNorepinephrine() != nullptr);
    assert(brain.getSerotonin() != nullptr);
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    
    std::cout << "All systems accessible and integrated successfully" << std::endl;
}

void testDevelopmentStageTransitions() {
    std::cout << "\n=== Testing Development Stage Transitions ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test initial stage properties
    assert(developmentSystem->getStage() == nlm::DevelopmentalStage::Initial);
    assert(developmentSystem->getPlasticityModifier() == 1.0f);
    assert(developmentSystem->getCriticalPeriodProgress() == 0.0f);
    
    // Simulate some development steps
    // Note: In real implementation, development stages advance based on time
    
    // Test stage advancement
    developmentSystem->setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(developmentSystem->getStage() == nlm::DevelopmentalStage::CriticalPeriod);
    assert(developmentSystem->isCriticalPeriod() == true);
    assert(developmentSystem->getStageName() == std::string("CriticalPeriod"));
    
    std::cout << "Stage transitions working correctly" << std::endl;
}

void testDevelopmentSystemPlasticityModulation() {
    std::cout << "\n=== Testing Development System Plasticity Modulation ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test plasticity modifiers for different stages
    developmentSystem->setStage(nlm::DevelopmentalStage::Initial);
    assert(developmentSystem->getPlasticityModifier() == 1.0f);
    
    developmentSystem->setStage(nlm::DevelopmentalStage::CriticalPeriod);
    assert(developmentSystem->getPlasticityModifier() == 0.8f);
    
    developmentSystem->setStage(nlm::DevelopmentalStage::Maturation);
    assert(developmentSystem->getPlasticityModifier() == 0.5f);
    
    developmentSystem->setStage(nlm::DevelopmentalStage::Adult);
    assert(developmentSystem->getPlasticityModifier() == 0.2f);
    
    developmentSystem->setStage(nlm::DevelopmentalStage::Aging);
    assert(developmentSystem->getPlasticityModifier() == 0.1f);
    
    std::cout << "Plasticity modulation working correctly" << std::endl;
}

void testComprehensiveSystemIntegration() {
    std::cout << "\n=== Testing Comprehensive System Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Run a substantial simulation to test integration
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Periodically check development system state
        if (step % 200 == 0) {
            std::cout << "Step " << step << ": Development stage = " 
                      << developmentSystem->getStageName() 
                      << ", Age = " << developmentSystem->getDevelopmentalAge() << std::endl;
        }
    }
    
    std::cout << "Comprehensive integration test completed successfully" << std::endl;
}

void runAll() {
    testDevelopmentSystemIntegration();
    testDevelopmentStageTransitions();
    testDevelopmentSystemPlasticityModulation();
    testComprehensiveSystemIntegration();
    
    std::cout << "\n=== All Development Integration Tests Passed ===" << std::endl;
}

} // namespace test_development_integration

int main() {
    test_development_integration::runAll();
    return 0;
}
